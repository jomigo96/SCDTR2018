#ifndef CALIBRATION_H
#define CALIBRATION_H

#include "Arduino.h"
#include "comms.h"
#include "controller.h"

extern float K21, K12, K11, K22, o1, o2;
extern volatile byte isr_flag;
extern byte message_received;
extern const byte own_address;
extern message_t message;
extern const int sensor_pin, led_pin;
extern Controller controller;

enum State : short{

	start_cal,
	wait,
	cal0,
	cal0b,
	cal1,
	cal1b,
	cal2,
	cal2b,
	done,

};

inline void reset_counter(int *count, bool *flag){
	*count=0;
	*flag=false;
}

int run_calibration(int mode){

	const int led_on = 150;
	int count=0;
	bool flag_3s=false;
	bool flag_other=false;
	byte flag_calibration = 1;
	State state = start_cal;
	float L_other, L;
	int s;
	
	if(mode == 0){
		state = start_cal;
#ifdef DEBUG
		Serial.println("Starting calibration as master");
#endif
	}
	else if(mode == 1){
		state = cal0b;
#ifdef DEBUG
		Serial.println("Starting calibration as slave");
#endif
	}
	else
		return -1;


	while(1){

		if(message_received){ 

			// Handle message
			if(message.code == calibration_request){
				state = cal0b;
		        reset_counter(&count, &flag_3s);
			}
	
			if((message.code == data) || (message.code == acknowledge) ||(message.code == cont)){
				flag_other = true;
			}
	
			message_received = 0;
		}
	
		switch(state){
	
			case start_cal:
				message.code = calibration_request;
				message.address = own_address;
				send_message();
				state = cal0;
				reset_counter(&count, &flag_3s);
				break;
		
			case cal0:
				analogWrite(led_pin, 0);
				if(flag_3s){
					flag_3s = false;
		
					// Measuse background
					s = analogRead(sensor_pin);
					o1 = controller.compute_lux(s);

					state = cal1;
#ifdef DEBUG
					Serial.print("State cal0, background measured to ");
					Serial.println(o1);
#endif
				}
				break;
		
			case cal1:
				analogWrite(led_pin, 0);
				if(flag_other){
					flag_other=false;
		
					// Measuse influence of neighbour LED
					s = analogRead(sensor_pin);
					L = controller.compute_lux(s);
					K12 = (L-o1)/(led_on/255.0);
				
					message.code = acknowledge;
					message.address = own_address;
					send_message();

					state = cal2;
					reset_counter(&count, &flag_3s);
#ifdef DEBUG
					Serial.print("State cal1, LUX = ");
					Serial.println(L);
					Serial.print("K12 = ");
					Serial.println(K12);
#endif
				}
			break;

			case cal2:
				analogWrite(led_pin, led_on);
				if(flag_3s){
					flag_3s = false;
		
					// Measuse influence of own LED, and send it to neighbour
					s = analogRead(sensor_pin);
					L = controller.compute_lux(s);
		
					K11 = (L-o1)/(led_on/255.0);
		
					message.code = data;
					message.value[0] = o1;
					message.value[1] = K11;
					message.value[2] = K12;
					message.address = own_address;
					send_message();
#ifdef DEBUG
					Serial.print("State cal2, LUX =  ");
					Serial.println(L);
					Serial.print("K11 = ");
					Serial.println(K11);
#endif
				}
				if(flag_other){
					flag_other = false;

					o2 = message.value[0];
					K22 = message.value[1];
					K21 = message.value[2];
					
					state = done;

				}
			break;

			case cal0b:
				analogWrite(led_pin, 0);
				if(flag_3s){
					flag_3s = false;
		
					// Measuse background
					s = analogRead(sensor_pin);
					o1 = controller.compute_lux(s);
		
					state = cal1b;
					reset_counter(&count, &flag_3s);
		
#ifdef DEBUG
					Serial.print("State cal0b, background measured to ");
					Serial.println(o1);
#endif
				}
				break;
		
		
			case cal1b:
				analogWrite(led_pin, led_on);
				if(flag_3s){
					flag_3s = false;
		
					// Measuse influence of own LED, and send it to neighbour
					s = analogRead(sensor_pin);
					L = controller.compute_lux(s);
		
					K11 = (L-o1)/(led_on/255.0);
		
					message.code = cont;
					message.address = own_address;
					send_message();
				
#ifdef DEBUG
					Serial.print("State cal1b, LUX =  ");
					Serial.println(L);
					Serial.print("K11 = ");
					Serial.println(K11);
#endif
				}
				if(flag_other){
					flag_other = false;
					if(message.code == acknowledge)
						state = cal2b;
				}
			break;

			case cal2b:
				analogWrite(led_pin, 0);
				if(flag_other){
					flag_other=false;
		
					// Measuse influence of neighbour LED
					s = analogRead(sensor_pin);
					L = controller.compute_lux(s);
					K12 = (L-o1)/(led_on/255.0);
				
					o2 = message.value[0];
					K22 = message.value[1];
					K21 = message.value[2];

					message.address = own_address;
					message.code = data;
					message.value[0] = o1;
					message.value[1] = K11;
					message.value[2] = K12;
					send_message();

					state = done;
#ifdef DEBUG
					Serial.print("State cal2b, LUX = ");
					Serial.println(L);
					Serial.print("K12 = ");
					Serial.println(K12);
#endif
				}
			break;
		
			case done:

#ifdef DEBUG
				Serial.println("State done");
				Serial.print("o1 = ");
				Serial.println(o1);
				Serial.print("o2 = ");
				Serial.println(o2);
				Serial.print("K11 = ");
				Serial.println(K11);
				Serial.print("K22 = ");
				Serial.println(K22);
				Serial.print("K12 = ");
				Serial.println(K12);
				Serial.print("K21 = ");
				Serial.println(K21);
#endif
				return 0;

			default: break;
		}
	
		if(isr_flag){
			count++;
			if(count > 600){
				flag_3s=true;
				count=0;
			}
			isr_flag=0;
		}

	}
	return 0;
}


#endif /*CALIBRATION_H*/
