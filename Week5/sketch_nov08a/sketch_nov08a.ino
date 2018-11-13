#include <Wire.h>

const int own_address = 4;


void setup() {
  Serial.begin(9600);

  

  Wire.begin(own_address);
  Wire.onReceive(receiveEvent);

  TWAR |= 1; //Enable reception of broadcasts

}

void receiveEvent(int c){

  char str[20];
  int i = 0;
  
  while(Wire.available() > 0){
    str[i] = Wire.read();
    i++;
  }
  str[i]=0;
  Serial.println(str);
}

void loop() {

  char str[20];
  int i;

  i=0;
  while(Serial.available() > 0){
    str[i]=Serial.read();
    i++;
  }
  if(i>0){
    str[i]=0;
    Wire.beginTransmission(0); //Broadcast
    Wire.write(str);
    Wire.endTransmission();
  }
}
