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

// LED related
const int ledPin = 3;

// I/O
int value = 0;
int i;
char buffer[10];

// Interupts
volatile bool flag=0;


// Interupt service routine
ISR(TIMER1_COMPA_vec){

  // Check interrupt?
  flag=1;  
}

void setup() {

  cli();

  // Clear registers
  TCCR1A = 0; // Operating mode - CTC
  TCCR1B = 0; // Prescale = 1 - 62.5ns resolution
  TIMSK1 = 0; // Generate interrupt on match with OCR1A

  // Timer limits on this count
  OCR1A = 5*16 - 1; // 5*16*62.5ns = 5000 ns

  TCCR1A |= (1<<WGM11);
  TCCR1B |= (1<<CS10);
  TIMSK1 |= (1<<OCIE1A); 
  
  TCNT1 = 0; // Reset timer count
  sei();
  Serial.begin(9600);
}

void loop() {

/*
  // Read from Serial target LUX value
  i=0;
  while(Serial.available()){
    buffer[i]=Serial.read(); 
    i++;
  }
  if(i>0){
    buffer[i]=0;
    value=atoi(buffer);  
  }

  // Compute luminosity
  sensor_value = analogRead(sensor_pin);
  v = sensor_value*5.0/1.0230;
  R = (5-v/1000.0)/(v/1000.0/Raux);
  L = pow(10, (log10(R)-b)/m );
  //Serial.println("luminosity:");
  Serial.println(L);
*/
  // Control
  if(flag){

	  // blah blah

    Serial.println("Interrupt!"); 
    //analogWrite(ledPin, value);
    flag=0;
  }
}
