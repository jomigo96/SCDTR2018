/**
 * @file calibration.h
 * @author João Gonçalves, Daniel Schiffart, Francisco Castro
 * @date 12 Jan 2019
 * 
 * @brief Calibration procedure, in the form of the function start_calibration
 *
 * */
#ifndef CALIBRATION_H
#define CALIBRATION_H

#include "Arduino.h"
#include "comms.h"
#include "controller.h"

extern float K21, K12, K11, K22, o1, o2;
extern volatile byte isr_flag;
extern byte message_received;
extern const byte own_address;
extern message_t inc_message, out_message;
extern const int sensor_pin, led_pin;
extern Controller controller;

/*!
 * enum for state representation in the calibration protocol
 *
 * states with ´b´ are for the slave node, others for the master node
 * */
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

/*!
 * @brief Resets waiting time counter
 *
 * Uses the same timer as the control loop, with 5ms period, but the count
 * only triggers every 0.25 seconds. 
 * */
inline void reset_counter(int *count, bool *flag){
    *count=0;
    *flag=false;
}

/*! 
 * @brief Calibration procedure
 *
 * Nodes coordinate turning the LEDs on and off in order to measure values for
 * background illuminance (o1 o2), and the influence of every LED i on every
 * desk j, Kij, (K11 K12 K21 K22).
 * 
 * When calibration is started, both nodes switch off their LED and background
 * is measured. Then, the slave turns on its LED, and values are measured 
 * again. The slave turns off its LED and the master turns its LED on, and 
 * values are measured again. In the end, both nodes share the values they
 * calculated.
 *
 * No time-out implemented, if one node starts the calibration, it will wait 
 * forever for a reply.
 *
 * @param mode 0 for master, 1 for slave
 *
 * @return 0 for success, negative integer for failure
 * */
int run_calibration(int mode){

    const int led_on = 150;
    int count=0;
    bool flag_3s=false;
    bool flag_other=false;
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

        // Handles received message
        if(message_received){ 

            message_received = 0;

            if(inc_message.code == calibration_request){
#ifdef DEBUG
                Serial.println("Starting calibration as slave");
#endif
                state = cal0b;
                reset_counter(&count, &flag_3s);
            }
    
            if((inc_message.code == data) || (inc_message.code == acknowledge) 
                                                 ||(inc_message.code == cont)){
                flag_other = true;
            }
        }
    
        switch(state){
    
            case start_cal:
                // Requests calibration
                out_message.code = calibration_request;
                out_message.address = own_address;
                send_message();
                state = cal0;
                reset_counter(&count, &flag_3s);
                break;
        
            case cal0:
                // Switch off LED, wait, then measure background 
                analogWrite(led_pin, 0);
                if(flag_3s){
                    flag_3s = false;
        
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
                // Wait for signal, then measure illuminance
                analogWrite(led_pin, 0);
                if(flag_other){
                    flag_other=false;
        
                    // Measuse influence of neighbour LED
                    s = analogRead(sensor_pin);
                    L = controller.compute_lux(s);
                    K12 = (L-o1)/(led_on/255.0);
                
                    // Acknowledge, for synchronization of measurements
                    out_message.code = acknowledge;
                    out_message.address = own_address;
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
                // Turn LED on, wait, then measure illuminance
                analogWrite(led_pin, led_on);
                if(flag_3s){
                    flag_3s = false;
        
                    // Measuse influence of own LED, and send it to peer
                    s = analogRead(sensor_pin);
                    L = controller.compute_lux(s);
        
                    K11 = (L-o1)/(led_on/255.0);
        
                    out_message.code = data;
                    out_message.value[0] = o1;
                    out_message.value[1] = K11;
                    out_message.value[2] = K12;
                    out_message.address = own_address;
                    send_message();
#ifdef DEBUG
                    Serial.print("State cal2, LUX =  ");
                    Serial.println(L);
                    Serial.print("K11 = ");
                    Serial.println(K11);
#endif
                }
                // Receive peer data, then exit
                if(flag_other){
                    flag_other = false;

                    o2 = inc_message.value[0];
                    K22 = inc_message.value[1];
                    K21 = inc_message.value[2];
                    
                    state = done;

                }
            break;

            case cal0b:
                // Turn off LED, wait, then measure background
                analogWrite(led_pin, 0);
                if(flag_3s){
                    flag_3s = false;
        
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
                // Turn LED on, wait, then measure illuminance. Signal peer and
                // wait for the acknowledge.
                analogWrite(led_pin, led_on);
                if(flag_3s){
                    flag_3s = false;
        
                    // Measuse influence of own LED
                    s = analogRead(sensor_pin);
                    L = controller.compute_lux(s);
        
                    K11 = (L-o1)/(led_on/255.0);
        
                    out_message.code = cont;
                    out_message.address = own_address;
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
                    if(inc_message.code == acknowledge)
                        state = cal2b;
                }
            break;


            case cal2b:
                // Turn off LED, wait for signal then measure illuminance.
                // Receive peer's measured values and send our own, then exit.
                analogWrite(led_pin, 0);
                if(flag_other){
                    flag_other=false;
        
                    // Measuse influence of neighbour LED
                    s = analogRead(sensor_pin);
                    L = controller.compute_lux(s);
                    K12 = (L-o1)/(led_on/255.0);
                
                    o2 = inc_message.value[0];
                    K22 = inc_message.value[1];
                    K21 = inc_message.value[2];

                    out_message.address = own_address;
                    out_message.code = data;
                    out_message.value[0] = o1;
                    out_message.value[1] = K11;
                    out_message.value[2] = K12;
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
                Serial.println("Calibration finished");
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
            // 0.25s waiting period
            count++;
            if(count > 50){
                flag_3s=true;
                count=0;
            }
            isr_flag=0;
        }

    }
    return 0;
}


#endif /*CALIBRATION_H*/
