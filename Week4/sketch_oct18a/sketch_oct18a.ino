#include <avr/io.h>
#include <avr/interrupt.h>

// LDR related
const int sensor_pin = 0;
int sensor_value;
float v;
float R;
float L;
const float b = 5.8515;
const float m = -0.9355;
const float Raux = 10000;
const float C = 1e-6;
const float high = 80;
const float low = 40;

// LED related
const int ledPin = 3;
int value = 50;

// Interupts
volatile byte flag=0;
const int pin_switch=2;

// Control
volatile float target=80;
volatile float prev=40;
volatile float t=0;
float Ltarget;
float integral = 0;
float error = 0;
float error_keep = 0;
float u = 0;
float ff = 0;
const float KP = 0.01;
const float KI = 0.00001;
const float h = 0.0005;
const float epsilon = 0.7;

// Interupt service routine
ISR(TIMER1_COMPA_vect){

  flag=1;  
}

void switch_isr(){

  // Switch simulates a presence sensor, ie, toggles the target LUX
  if(target == high){
    target = low;
    prev = high;  
  }else{
    target = high;
    prev = low;  
  }
  t=0;
}

inline float deadzone(float error, float epsilon){
  
  if(error > epsilon){
    return error-epsilon;  
  }else if(error < epsilon){
    return error+epsilon;
  }else
    return 0;
}

inline int saturation(int value){

  int out;
  
  if(value > 255){
    out = 255;
  }else if(value < 0){
    out = 0;
  }else
    out = value;
  return out;
}

float simulation(){}

void setup() {

  
  cli();
  // Clear registers
  TCCR1A = 0; // Operating mode - CTC
  TCCR1B = 0; // Prescale = 1 - 62.5ns resolution
  TCNT1 = 0; // Reset timer count
  
  // Timer limits on this count
  OCR1A = 1000-1; // 1000*500ns = 500 micro.s

  TCCR1B |= (1<<WGM12);
  TCCR1B |= (0<<CS12)|(1<<CS11)|(0<<CS10); // prescaler 8
  TIMSK1 |= (1<<OCIE1A); 

  // Contact switch
  pinMode(pin_switch, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pin_switch), switch_isr, FALLING);
  
  sei();
  Serial.begin(230400);
}

void loop() {

  // Control
  if(flag){

    // Compute luminosity
    sensor_value = analogRead(sensor_pin);
    v = sensor_value*5.0/1.0230;
    R = (5-v/1000.0)/(v/1000.0/Raux);
    L = pow(10, (log10(R)-b)/m );

    //Feed-forward
    ff = target * 0.5228758;

    //Simulator
    //¯\_(ツ)_/¯
    Ltarget = target - (target - prev)*exp(-t/((R+Raux)*C));

    //Feed-back
    error = deadzone(Ltarget - L, epsilon);
    integral = integral + h/2.0*(error + error_keep);
    u += (integral*KI + error)*KP;
    value = ff + round(u);

    //Serial.println(u);
    
    //Control output
    analogWrite(ledPin, saturation(value));
    
    //Serial.println(saturation(value));
    //Serial.println(Ltarget);
    Serial.println(L);
    
    t += h;
    
    //Anti windup, stop integrating
    if(value > 255){
      u = 255 - ff;
      integral = integral - h/2.0*(error+error_keep);  
    }else if(value<0){
      u = -ff;     
      integral = integral - h/2.0*(error+error_keep); 
    }
    
    flag=0;
    error_keep = error;
  }
  
  
}
