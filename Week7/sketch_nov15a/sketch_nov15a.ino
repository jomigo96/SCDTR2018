#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>

//calibration
volatile byte flag_calibration = 1;

// LDR related
float v;
float R, R_12;
float L, L_12;
const float b = 5.8515;
const float m = -0.9355;
const float Raux = 100000;

// Le tensao dos LDR em mV
const int sensor_pin = 1;
int sensor_value;
int v1, v_12, k12;
char str[128];

// LED related
const int ledPin = 3;
const int led_ON = 150;
const int led_OFF = 0;
int value = 0;

// Interupts
volatile byte flag=0;
volatile int count=0;

// I2C
const byte own_address = 0x02; //The I2C address of this node, unique for each arduino. 
typedef struct message{
    bool turn;
    byte address;
}message_t; //Message struct. Maximum of 32 bytes?

message_t last_message; //Actual message in memory
volatile bool message_received = false; //Flag that shows that a message has been received
bool to_send = true; //Flag that shows that it is this node's turn to send the next message

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
}

void loop() {

 if(flag_calibration == 0) {
  sensor_value = analogRead(sensor_pin);

  v = sensor_value*5.0/1.0230;
  R = (5-v/1000.0)/(v/1000.0/Raux);
  L = pow(10, (log10(R)-b)/m );

  v_12 = sensor_value*5.0/255;
  R_12 = (5-v_12/1000.0)/(v_12/1000.0/Raux);
  L_12 = pow(10, (log10(R_12)-b)/m );
  
  k12 = L/L_12;
  flag_calibration = 1;
  Serial.println("Calibration");
  Serial.println(k12);
 }
 
 if(message_received){ //When a message is received, this node turns it's led on, and is resposnible to send the next message.
  Serial.println("Received a message!"); 
  
  if(last_message.turn){
    analogWrite(ledPin, led_ON);  
  }else{
    analogWrite(ledPin, led_OFF);  
  }
  message_received = false;
  to_send=true;
  flag=false;
 }

 if(flag){
  if(to_send){ //Sends message for the other node to turn on his led

    last_message.turn=true;
    last_message.address=own_address;
    Wire.beginTransmission(0);
    Wire.write((char*)&last_message, sizeof(message_t));
    Wire.endTransmission();
    to_send=false;
    analogWrite(ledPin, led_OFF); //Switches off own led
  }
  flag=false;
 }
}
