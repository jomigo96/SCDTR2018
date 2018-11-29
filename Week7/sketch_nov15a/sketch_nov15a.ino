#define DEBUG
#include <avr/io.h>
#include <avr/interrupt.h>
#include "comms.h"
#include "calibration.h"
#include "controller.h"

/**************************************************************************/
// Variables

// LED 
const int ledPin = 3;
int pwm_value = 0;

// Interupts
volatile byte isr_flag=0;

// I2C
const byte own_address = 0x09; 
message_t message; // maybe these need to be volatile?
byte message_received = false; 

// LDR 
const int sensor_pin = 0;

// Control
float K21 = 0;
float K12 = 0;
float K11 = 0;
float K22 = 0;
float o1 = 0; // Background illuminance levels
float o2 = 0;
Controller controller = Controller();

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
#endif

  run_calibration(0);

}

/**************************************************************************/
// Main loop

void loop() {

	if(message_received){

		message_received = false;

		if(message.code == calibration_request)
			run_calibration(1);
	}
	
}
