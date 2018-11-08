#include <Wire.h>

const int own_address = 4;
const int peer_address = 5;

void setup() {
  Serial.begin(9600);
  Wire.begin(own_address);
  Wire.onReceive(receiveEvent);
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

  char c;

  while(Serial.available() > 0){
    c=Serial.read();
    Wire.beginTransmission(peer_address);
    Wire.write(c);
    Wire.endTransmission();
  }

  

}
