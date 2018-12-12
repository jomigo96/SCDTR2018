#ifndef COMMS_H
#define COMMS_H

#include "Arduino.h"
#include <Wire.h>



enum MsgCode : uint8_t{

	calibration_request,
	data,
	cont,
	acknowledge,
	consensus_data,
	consensus_stop,
	sampling_time_data,
  none,

};

typedef struct msg{
	MsgCode code; // 1 byte
	uint8_t address; // 1 byte
	uint8_t aux1; // 1 byte
	uint8_t aux2; // 1 byte
	float value[4]; // 4 x 4 = 16 bytes

}message_t; //size fixed to 20 bytes

extern message_t inc_message, out_message;
extern byte message_received;

void receiveEvent(int c){ //Function that is called when a I2C message is received

	char buf[20]; //Local buffer
	int i=0;


	while((Wire.available() > 0)&&(i < sizeof(message_t))){
		buf[i]=Wire.read();
		i++;
	}

	memcpy(&inc_message, buf, sizeof(message_t)); 
	
	message_received=true; 

#ifdef DEBUG_MSG
	Serial.println("Incoming message:");
	Serial.println(inc_message.code);
	Serial.println(inc_message.address);
	Serial.println(inc_message.value[0]);
	Serial.println(inc_message.value[1]);
	Serial.println(inc_message.value[2]);
	Serial.println(inc_message.value[3]);
	Serial.println(inc_message.aux1);
	Serial.println(inc_message.aux2);
#endif
}

void send_message(){

	Wire.beginTransmission(0);
	Wire.write((char*)&out_message, sizeof(message_t));
	Wire.endTransmission();
#ifdef DEBUG_MSG
	Serial.println("Outgoing message:");
	Serial.println(out_message.code);
	Serial.println(out_message.address);
	Serial.println(out_message.value[0]);
	Serial.println(out_message.value[1]);
	Serial.println(out_message.value[2]);
	Serial.println(out_message.value[3]);
	Serial.println(out_message.aux1);
	Serial.println(out_message.aux2);
#endif
 
}

inline void send_sample_time_data(const byte &address, const int &dimming, const int &lower_bound,			   	const float &L, const float &o1, const float &target, const float& c){

#ifdef TIMING
	long t1, t2;
	t1 = micros();
#endif

	out_message.address = address;
	out_message.code = sampling_time_data;
	out_message.aux1 = dimming;
	if (lower_bound == 80)
		out_message.aux1 |= (1<<7);
	out_message.aux2 = lower_bound;
	out_message.value[0] = L;
	out_message.value[1] = o1;
	out_message.value[2] = target;
	out_message.value[3] = c * dimming * 255.0;

	send_message();

#ifdef TIMING
	t2 = micros();
	Serial.print("Data-forwarding time taken (micros): ");
	Serial.println(t2-t1);
#endif
}


inline void send_consensus_iteration_data(const float& d1, const float& d2, const byte &address){

#ifdef TIMING
	long t1, t2;
	t1 = micros();
#endif

	out_message.address = address;
	out_message.code = consensus_data;
	out_message.value[0] = d1;
	out_message.value[1] = d2;

	send_message();

#ifdef TIMING
	t2 = micros();
	Serial.print("Data-forwarding time taken (micros): ");
	Serial.println(t2-t1);
#endif
}


#endif
