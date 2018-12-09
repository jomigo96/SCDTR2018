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
	sampling_time_data,

};

typedef struct msg{
	MsgCode code; // 1 byte
	uint8_t address; // 1 byte
	uint8_t aux1; // 1 byte
	uint8_t aux2; // 1 byte
	float value[4]; // 4 x 4 = 16 bytes

}message_t; //size fixed to 20 bytes

extern message_t message;
extern byte message_received;

void receiveEvent(int c){ //Function that is called when a I2C message is received

	char buf[20]; //Local buffer
	int i=0;

	while((Wire.available() > 0)&&(i < sizeof(message_t))){
		buf[i]=Wire.read();
		i++;
	}

	memcpy(&message, buf, sizeof(message_t)); 
	
	message_received=true; 

#ifdef DEBUG_MSG
	Serial.println("Incoming message:");
	Serial.println(message.code);
	Serial.println(message.address);
	Serial.println(message.value[0]);
	Serial.println(message.value[1]);
	Serial.println(message.value[2]);
	Serial.println(message.value[3]);
	Serial.println(message.aux1);
	Serial.println(message.aux2);
#endif
}

void send_message(){

	Wire.beginTransmission(0);
	Wire.write((char*)&message, sizeof(message_t));
	Wire.endTransmission();
#ifdef DEBUG_MSG
	Serial.println("Outgoing message:");
	Serial.println(message.code);
	Serial.println(message.address);
	Serial.println(message.value[0]);
	Serial.println(message.value[1]);
	Serial.println(message.value[2]);
	Serial.println(message.value[3]);
	Serial.println(message.aux1);
	Serial.println(message.aux2);
#endif
 
}

inline void send_sample_time_data(const byte &address, const int &dimming, const int &lower_bound,			   	const float &L, const float &o1, const float &target, const float& c){

#ifdef TIMING
	long t1, t2;
	t1 = micros();
#endif

	message.address = address;
	message.code = sampling_time_data;
	message.aux1 = dimming;
	if (lower_bound == 80)
		message.aux1 |= (1<<7);
	message.aux2 = lower_bound;
	message.value[0] = L;
	message.value[1] = o1;
	message.value[2] = target;
	message.value[3] = c * dimming * 255.0;

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

	message.address = address;
	message.code = consensus_data;
	message.value[0] = d1;
	message.value[1] = d2;

  send_message();

#ifdef TIMING
  t2 = micros();
  Serial.print("Data-forwarding time taken (micros): ");
  Serial.println(t2-t1);
#endif
}


#endif
