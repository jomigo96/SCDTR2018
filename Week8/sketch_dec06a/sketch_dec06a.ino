#define NODE1
#define DEBUG
#define SUPRESS_LUX
//#define DEBUG_MSG
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
volatile byte isr_flag=0;

// I2C

#ifdef NODE1
const byte own_address = 0x01; 
#else
const byte own_address = 0x09;
#endif

message_t message; // maybe these need to be volatile?
byte message_received = false; 

// LDR 
const int sensor_pin = 0;

// Control
float K21 = 1;
float K12 = 1;
float K11 = 200;
float K22 = 200;
float o1 = 0; // Background illuminance levels
float o2 = 0;
float target, L;
const float c1=1, c2=1;
float d1, d2;
const float tol = 0.1;
byte flag_updated = 1;

#ifdef NODE1
Controller controller = Controller(5.8515, -0.9355, 10000, 1e-6);
#else
Controller controller = Controller(7.3250, -1.4760, 10000, 1e-6);
#endif

int lower_bound = 80;



/**************************************************************************/
// Functions

// Interupt service routine, Period of 5ms
ISR(TIMER1_COMPA_vect){
	isr_flag=true;
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

		if(message.code == calibration_request)
			run_calibration(1);

		if(message.code == consensus_data){
			controller.update(message.value[1], message.value[0]);
			flag_updated = 1;
		}
	}

	//controller.set_lower_bound(80);
	if(flag_updated){
		flag_updated=0;
		controller.consensus_iteration();
		controller.get_dimmings(d1, d2);
		send_consensus_iteration_data(d1, d2, own_address);
	
#ifdef DEBUG
		Serial.print("d1=");
		Serial.println(d1);
		Serial.print("d2=");
		Serial.println(d2);
#endif
	

		target = o1 + K11*d1 + K12*d2;
		if(target < 80){
			Serial.println("Something wrong");
			target = 80;
      delay(5000);
		}
	}

	if(isr_flag){
		isr_flag=0;

		controller.PID_control(target, dimming, L);

		//send_sample_time_data(own_address, dimming, lower_bound, L, o1, target, c1);
	}
}
