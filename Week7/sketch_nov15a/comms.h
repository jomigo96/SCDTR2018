#ifndef COMMS_H
#define COMMS_H

#include "Arduino.h"
#include <Wire.h>



enum MsgCode : uint8_t{

	calibration_request,
	data,
	cont,
	acknowledge,

};

typedef struct msg{
	MsgCode code; // 1 byte
	uint8_t address; // 1 byte
	uint8_t aux1; // 1 byte
	uint8_t aux2; // 1 byte
	float value[7]; // 7 x 4 = 28 bytes

}message_t; // size should be 32 bytes

extern message_t message;
extern byte message_received;

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
  Serial.println(message.code);
  Serial.println(message.address);
  Serial.println(message.value[0]);
  Serial.println(message.value[1]);
  Serial.println(message.value[2]);
  Serial.println(message.value[3]);
  Serial.println(message.value[4]);
  Serial.println(message.value[5]);
  Serial.println(message.value[6]);
  Serial.println(message.value[7]);
  Serial.println(message.aux1);
  Serial.println(message.aux2);
#endif
}

void send_message(){

	Wire.beginTransmission(0);
	Wire.write((char*)&message, sizeof(message_t));
	Wire.endTransmission();
#ifdef DEBUG
	Serial.println("Outgoing message:");
	Serial.println(message.code);
	Serial.println(message.address);
	Serial.println(message.value[0]);
  Serial.println(message.value[1]);
  Serial.println(message.value[2]);
  Serial.println(message.value[3]);
  Serial.println(message.value[4]);
  Serial.println(message.value[5]);
  Serial.println(message.value[6]);
  Serial.println(message.value[7]);
  Serial.println(message.aux1);
  Serial.println(message.aux2);
#endif
 
}

#endif
