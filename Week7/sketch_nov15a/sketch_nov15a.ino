#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#define DEBUG

// LED related
const int ledPin = 3;
int value = 0;
const int led_on = 150;

// Interupts
volatile byte flag=0;

// I2C
enum State : short{

	start_cal,
	cal0,
	cal0b,
	cal1,
	cal1b,
	cal2,
	cal2b,
	control,
	data,

};

typedef struct message{
    State state;
    byte address;
	float value;
}message_t; 

const byte own_address = 0x02; 

message_t message; // maybe needs to be volatile
volatile bool message_received = false; 

// LDR 
const int sensor_pin = 0;
int s;
float L;
const float b = 5.3060;
const float m = -0.7724;
const float Raux = 10000;
const float C = 1e-6;

// State machine
State state = start_cal;
int count=0;
bool flag_3s=false;
bool flag_other=false;

// Control
float K21 = 0;
float background = 0;
float own_gain=0.522;
float L_other=false;

/**************************************************************************/

// Interupt service routine, Period of 5ms
ISR(TIMER1_COMPA_vect){
	flag=true;
}

inline void reset_counter(void){
	count=0;
	flag_3s=false;
}

float compute_lux(int s){

  float v, R, L;
  
	v = s * 5.0/1.0230;
	R = (5-v/1000.0)/(v/1000.0/Raux);
	L = pow(10, (log10(R)-b)/m );
	return L;
}

void receiveEvent(int c){ //Function that is called when a I2C message is received

	char buf[32]; //Local buffer
	int i=0;

	while((Wire.available() > 0)&&(i < sizeof(message_t))){ 
		buf[i]=Wire.read();
		i++;
	}

  memcpy(&message, buf, sizeof(message_t)); 
	
	message_received=true; 
#ifdef DEBUG
  Serial.println("Incoming message:");
  Serial.println(message.state);
  Serial.println(message.address);
  Serial.println(message.value);
#endif
}

void send_message(){

	Wire.beginTransmission(0);
	Wire.write((char*)&message, sizeof(message_t));
	Wire.endTransmission();
#ifdef DEBUG
  Serial.println("Outgoing message:");
  Serial.println(message.state);
  Serial.println(message.address);
  Serial.println(message.value);
#endif
 
}

/**************************************************************************/

void setup() {

  // Set Timer1 to raise an interruption with the sampling time of 0.0005seconds
  cli();
  // Clear registers
  TCCR1A = 0; // Operating mode - CTC
  TCCR1B = 0; // Prescale = 1 - 62.5ns resolution
  TCNT1 = 0; // Reset timer count
  
  // Timer limits on this count
  OCR1A = 10000-1; // 10000*500ns = 5000 micro.s

  TCCR1B |= (1<<WGM12);
  TCCR1B |= (0<<CS12)|(1<<CS11)|(0<<CS10); // prescaler 8
  TIMSK1 |= (1<<OCIE1A); 
  
  sei();

  // I2C
  Wire.begin(own_address);
  Wire.onReceive(receiveEvent); //Signals that receiveEvent() should be called when a message is received
  TWAR |= 1; //Enable reception of broadcasts
  
  Serial.begin(230400);
}

/**************************************************************************/

void loop() {

	if(message_received){ 

		// Handle message
		if(message.state == start_cal)
			state = cal0b;

		if((message.state == data) && ((state == cal1)||(state == cal2b))){
			flag_other = true;
			L_other = message.value;
		}

		message_received = false;
	}

	switch(state){

		case start_cal:
			message.state=start_cal;
			message.address=own_address;
			send_message();
			state = cal0;
			reset_counter();
			break;
	
		case cal0:
			analogWrite(ledPin, 0);
			if(flag_3s){
				flag_3s=false;
	
				// Measuse background
				s = analogRead(sensor_pin);
				background = compute_lux(s);
	
				state = cal1;
#ifdef DEBUG
				Serial.print("State cal0, background measured to ");
				Serial.println(background);
#endif
			}
		break;
	
		case cal0b:
			analogWrite(ledPin, led_on);
			if(flag_3s){
				flag_3s=false;
	
				// Measuse background
				background = analogRead(sensor_pin);
	
				background = L;
	
				state = cal1b;
				reset_counter();
	
#ifdef DEBUG
				Serial.print("State cal0b, background measured to ");
				Serial.println(background);
#endif
			}
			break;
	
		case cal1:
			analogWrite(ledPin, 0);
			if(flag_other){
				flag_other=false;
	
				// Measuse influence of neighbour LED
				s = analogRead(sensor_pin);
				L = compute_lux(s);
	
				K21 = L/L_other;
			
				state = cal2;
				reset_counter();
#ifdef DEBUG
				Serial.print("State cal1, LUX = ");
				Serial.println(L);
				Serial.print("Peer LUX = ");
				Serial.println(L_other);
#endif
			}
		break;
	
		case cal1b:
			analogWrite(ledPin, led_on);
			if(flag_3s){
				flag_3s=false;
	
				// Measuse influence of own LED, and send it to neighbour
				s = analogRead(sensor_pin);
				L = compute_lux(s);
	
				own_gain = L/led_on;
	
				message.state = data;
				message.value = L;
				message.address = own_address;
				send_message();
			
				state = cal2b;
#ifdef DEBUG
				Serial.print("State cal1b, LUX =  ");
				Serial.println(L);
#endif
			}
		break;
	
		case cal2:
			analogWrite(ledPin, led_on);
			if(flag_3s){
				flag_3s = false;
	
				// Measuse influence of own LED, and send it to neighbour
				s = analogRead(sensor_pin);
				L = compute_lux(s);
	
				own_gain = L/led_on;
	
				message.state = data;
				message.value = L;
				message.address = own_address;
				send_message();
	
				state = control;
#ifdef DEBUG
				Serial.print("State cal2, LUX =  ");
				Serial.println(L);
#endif
			}
		break;
	
		case cal2b:
			analogWrite(ledPin, 0);
			if(flag_other){
				flag_other=false;
	
				// Measuse influence of neighbour LED
				s = analogRead(sensor_pin);
				L = compute_lux(s);
	
				K21 = L/L_other;
			
				state = control;
#ifdef DEBUG
				Serial.print("State cal2b, LUX = ");
				Serial.println(L);
				Serial.print("Peer LUX = ");
				Serial.println(L_other);
#endif
			}
		break;
	
		default: break;
	}

	if(flag){
		count++;
		if(count > 2000){
			flag_3s=true;
			count=0;
		}
	flag=false;

#ifdef DEBUG
		if(state == control){
      if(flag_3s){
  			Serial.println("Calibration done");
  			Serial.print("K21 = ");
  			Serial.println(K21);
  			Serial.print("Background = ");
  			Serial.println(background);
  			Serial.print("Self gain = ");
  			Serial.println(own_gain);
        flag_3s=false;
      }
		}
#endif /*DEBUG*/
	}
}
