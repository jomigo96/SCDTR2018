/**
 * @file comms.h
 * @author João Gonçalves, Daniel Schiffart, Francisco Castro
 * @date 12 Jan 2019
 * 
 * @brief Communication protocols
 *
 * */

#ifndef COMMS_H
#define COMMS_H

#include "Arduino.h"
#include <Wire.h>

/*!
 * enum for idenfifying message contents/purpose
 * */
enum MsgCode : uint8_t{

    calibration_request,
    data,
    cont,
    acknowledge,
    consensus_data,
    consensus_stop,
    consensus_new,
    sampling_time_data,
    none,
};

/*!
 * @brief Message data structure, with size fixed to 16 bytes
 * */
typedef struct msg{
    MsgCode code; //!< Message code
    uint8_t address; //!< Sender's I2C address
    uint8_t aux1; //!< field for data
    uint8_t aux2; //!< field for data
    float value[3]; //!< field for data

}message_t; 

extern message_t inc_message, out_message;
extern byte message_received;

/*!
 * @brief Function that is called when a I2C message is received
 * 
 * Copies message contents to inc_message and sets a flag. Ignores 
 * sampling_time_data type of messages.
 * If the DEBUG_MSG macro is defined, message contents are printed to the 
 * serial monitor.
 * */
void receiveEvent(int c){ 

    char buf[16]; //Local buffer
    int i=0;

    while((Wire.available() > 0)&&(i < sizeof(message_t))){
        buf[i]=Wire.read();
        i++;
    }
    if((uint8_t)buf[0] == sampling_time_data){
        return; // No need for this
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
    Serial.println(inc_message.aux1);
    Serial.println(inc_message.aux2);
#endif
}

/*!
 * @brief Sends I2C message, contained in out_message
 *
 * If the DEBUG_MSG macro is defined, message contents are printed to the 
 * serial monitor.
 *
 * */
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
    Serial.println(out_message.aux1);
    Serial.println(out_message.aux2);
#endif
 
}

/*!
 * @brief Sends data for data collection purposes
 *
 * If the TIMING macro is defined, elapsed time will be printed to the serial
 * monitor.
 * */
inline void send_sample_time_data(const byte &address, const int &dimming, 
                   const int &lower_bound, const float &L, const float &target, 
                                                               const float& c){

#ifdef TIMING
    long t1, t2;
    t1 = micros();
#endif

    out_message.address = address;
    out_message.code = sampling_time_data;
    out_message.aux1 = dimming;
    out_message.aux2 = lower_bound;
    out_message.value[0] = L;
    out_message.value[1] = c * dimming * 2.55;
    out_message.value[2] = target;

    send_message();

#ifdef TIMING
    t2 = micros();
    Serial.print("Data-forwarding time taken (micros): ");
    Serial.println(t2-t1);
#endif
}

/*!
 * @brief Sends data to be used in distributed optimization
 *
 * If the TIMING macro is defined, elapsed time will be printed to the serial
 * monitor.
 * */
inline void send_consensus_iteration_data(MsgCode code, const float& d1, 
                                         const float& d2, const byte &address){

#ifdef TIMING
    long t1, t2;
    t1 = micros();
#endif

    out_message.address = address;
    out_message.code = code;
    out_message.value[0] = d1;
    out_message.value[1] = d2;

    send_message();

#ifdef TIMING
    t2 = micros();
    Serial.print("Consensus forwarding time taken (micros): ");
    Serial.println(t2-t1);
#endif
}


#endif
