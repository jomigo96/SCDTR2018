#define NODE2
#define DEBUG
#define SUPRESS_LUX
//#define DEBUG_MSG
//#define TIMING
#include <avr/io.h>
#include <avr/interrupt.h>
#include "comms.h"
#include "calibration.h"
#include "controller.h"

/**************************************************************************/
// Variables

// LED 
const int led_pin = 3;
int dimming = 0;

// Interupts
volatile byte isr_flag = 0;
const int pin_switch = 2;
uint32_t debouncer = 0;
byte flag_bound_changed = 0;

// I2C
#ifdef NODE1
const byte own_address = 0x01; 
#else
const byte own_address = 0x09;
#endif

message_t out_message, inc_message; // maybe these need to be volatile?
byte message_received = false; 

// LDR 
const int sensor_pin = 0;
const int lux_high = 250;
const int lux_low = 120;

#ifdef NODE1
Controller controller = Controller(5.8515, -0.9355, 10000, 1e-6);
float K11=326.84, K22=411.81, K12=92.72, K21=109.37, o1=4.88, o2=6.82; //Typical values
const float c1=1;
#else
Controller controller = Controller(7.3250, -1.4760, 10000, 1e-6);
float K22=326.84, K11=411.81, K21=92.72, K12=109.37, o2=4.88, o1=6.82; //Typical values
const float c1=4;
#endif

// Control
float L;
float d1, d2;
byte flag_updated = 1;
int lower_bound = lux_high;
float target = lower_bound;
float d_sol = (lower_bound-o1)/K11*100;

/**************************************************************************/
// Functions

// Interupt service routine, Period of 5ms
ISR(TIMER1_COMPA_vect){
	isr_flag=true;
}

void switch_isr(){
	// Debouncing 
	if(debouncer < 200)
		return;
	debouncer = 0;

	if(lower_bound == lux_low){
		lower_bound = lux_high;
	}else{
		lower_bound = lux_low;
	}
	flag_bound_changed = 1;

#ifdef DEBUG
	Serial.print("Lower bound set to: ");
	Serial.println(lower_bound);
#endif
}

/**************************************************************************/
// Setup

void setup() {
  
	// Set Timer1 to raise an interruption with the sampling time of 0.005seconds
	cli(); // disable global interrupts
	// Clear registers
	TCCR1A = 0; // Operating mode - CTC
	TCCR1B = 0; // Prescale = 1 - 62.5ns resolution
	TCNT1 = 0; // Reset timer count

	// Timer limits on this count
	OCR1A = 10000-1; // 10000*500ns = 5000 micro.s

	TCCR1B |= (1<<WGM12);
	TCCR1B |= (0<<CS12)|(1<<CS11)|(0<<CS10); // prescaler 8
	TIMSK1 |= (1<<OCIE1A); 
  
	sei(); // enable global interrupts

	// I2C
	Wire.begin(own_address);
	Wire.onReceive(receiveEvent); //Signals that receiveEvent() should be called when a message is received
	TWAR |= 1; //Enable reception of broadcasts

	// Occupation toggle button
	pinMode(pin_switch, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(pin_switch), switch_isr, FALLING);
  
	Serial.begin(230400);

#ifdef DEBUG
	Serial.println("--- Arduino restarted ---");

#ifdef NODE1
  Serial.println("This is node 1");
#else
  Serial.println("This is node 2");
#endif 
  
#endif 

	run_calibration(0);

}

/**************************************************************************/
// Main loop

void loop() {

	if(message_received){ // Handle messages
		message_received = false;

		if(inc_message.code == calibration_request){
			run_calibration(1);
			controller.reset_consensus();
			flag_updated=1;
		}

		if(inc_message.code == consensus_data){
			flag_updated = controller.update(inc_message.value[1], inc_message.value[0]);
			if(!flag_updated){
				controller.get_dimmings(&d1, &d2);
				target = o1 + K11*d1/100.0 + K12*d2/100.0;
				d_sol = d1;
				out_message.code = consensus_stop;
				send_message();
#ifdef DEBUG
				Serial.println("Consensus converged");
				Serial.print("d1=");
				Serial.println(d1);
				Serial.print("d2=");
				Serial.println(d2);
#endif
			}
		}
		if(inc_message.code == consensus_stop){
			controller.get_dimmings(&d1, &d2);
			target = o1 + K11*d1/100.0 + K12*d2/100.0;
			d_sol=d1;
			inc_message.code = none;
#ifdef DEBUG
			Serial.println("Consensus converged");
			Serial.print("d1=");
			Serial.println(d1);
			Serial.print("d2=");
			Serial.println(d2);
#endif
		}
		if(inc_message.code == consensus_new){
			controller.reset_consensus();
			controller.consensus_iteration();
			controller.get_dimmings(&d1, &d2);
			controller.update(inc_message.value[1], inc_message.value[0]);
			send_consensus_iteration_data(consensus_data, d1, d2, own_address);
			flag_updated = 1;
#ifdef DEBUG
			Serial.println("New consensus started.");
#endif
		}
	}

	// Consensus iteration
	if(flag_updated){
		flag_updated=0;
		controller.consensus_iteration();
		controller.get_dimmings(&d1, &d2);
		send_consensus_iteration_data(consensus_data,d1, d2, own_address);
	
#ifdef DEBUGGY
		Serial.print("d1av=");
		Serial.println(d1);
		Serial.print("d2av=");
		Serial.println(d2);
#endif
	}
	if(flag_bound_changed){
		flag_bound_changed = 0;
		controller.set_lower_bound(lower_bound);
		controller.reset_consensus();
		controller.consensus_iteration();
		controller.get_dimmings(&d1, &d2);
		send_consensus_iteration_data(consensus_new, d1, d2, own_address);
	}

	// Controller
	if(isr_flag){
		isr_flag=0;

		controller.PID_control(target, d_sol/100.0, &dimming, &L);

		send_sample_time_data(own_address, dimming, lower_bound, L, o1, target, c1);
		debouncer++;
	}
}
