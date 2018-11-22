#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>

// LED related
const int ledPin = 3;
int value = 0;

// Interupts
volatile byte flag=0;
volatile int count=0;

// I2C

enum State : short{

  calibrate_0,
  calibrate_1,
  calibrate_2,
  data,
  
};

typedef struct message{
    State state;
    byte address;
}message_t; //Message struct. Maximum of 32 bytes?

const byte own_address = 0x01; //The I2C address of this node, unique for each arduino. 


message_t last_message; //Actual message in memory
volatile bool message_received = false; //Flag that shows that a message has been received
bool to_send = true; //Flag that shows that it is this node's turn to send the next message

// LDR 
const int sensor_pin = 0;
int s1, s2, s3;
float v;
float R;
float L;
const float b = 5.3060;
const float m = -0.7724;
const float Raux = 10000;
const float C = 1e-6;

// State machine
State state = calibrate_0;

// Interupt service routine, turns flag true every few seconds
ISR(TIMER1_COMPA_vect){
 count++;
 if(count > 1000){
  flag=1;  
  count = 0;
 }
}

void receiveEvent(int c){ //Function that is called when a I2C message is received

  char buf[20]; //Local buffer
  int i=0;

    while((Wire.available() > 0)&&(i < sizeof(message_t))){ //reads at most the size of message_t
      buf[i]=Wire.read();
      i++;
    }
    memcpy(&last_message, buf, sizeof(message_t)); //Copies the message from buffer to global memory
    message_received=true; //signals that a message has been received
}

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

void loop() {

 if(message_received){ 
  //Serial.println("Received a message!"); 

  message_received = false;
  to_send=true;
  flag=false;
 }

 switch(state){

  case calibrate_0:
    break;
  case calibrate_1:
    break;
  case calibrate_2:
    break;
  case data:
    break;
 }

 if(flag){
  if(to_send){ //Sends message for the other node to turn on his led

    last_message.turn=true;
    last_message.address=own_address;
    Wire.beginTransmission(0);
    Wire.write((char*)&last_message, sizeof(message_t));
    Wire.endTransmission();
    to_send=false;
    analogWrite(ledPin, 0); //Switches off own led
  }
  flag=false;
 }
}
