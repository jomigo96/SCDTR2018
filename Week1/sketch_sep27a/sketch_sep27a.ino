const int sensorPin = 0;

int sensorValue;

void setup(){


  Serial.begin(9600);
}

void loop(){

  sensorValue = analogRead(sensorPin);
  Serial.println(sensorValue);
  delay(1000);

}
