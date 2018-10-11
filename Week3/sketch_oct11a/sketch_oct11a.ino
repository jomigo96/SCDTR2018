// Le tensao dos LDR em mV
const int sensorPin1 = 0;
const int ledPin = 3;

int sensorValue1 = 0;

int v1;

int led_value = 0;

char str[128];

int i=0;

void setup(){

  Serial.begin(230400);
}



void loop(){

  sensorValue1 = analogRead(sensorPin1);
  
  v1 = sensorValue1*5.0/1.0230;

  sprintf(str,"%d - %ld - %d", v1, micros(), led_value);
  Serial.println(str);
  

  if(i>=1000){
    led_value +=50;
    Serial.println("Step");
    i=0;   
  }
  if(led_value >= 250){
    led_value=0;  
  }
  analogWrite(ledPin, led_value);

  i++;
  delay(5);
}
