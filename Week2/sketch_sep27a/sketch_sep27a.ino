// Le tensao dos LDR em mV
const int sensorPin1 = 0;
const int sensorPin2 = 1;

int sensorValue1;
int sensorValue2;

int v1, v2;

char str[128];

void setup(){


  Serial.begin(9600);
}



void loop(){



  sensorValue1 = analogRead(sensorPin1);
  sensorValue2 = analogRead(sensorPin2);

  v1 = sensorValue1*5.0/1.0230;
  v2 = sensorValue2*5.0/1.0230;

  //sprintf(str, "LDR 1: %d -- LDR 2: %d (V)", v1, v2);
  
  Serial.println("LDR1: (pin 0)");
  Serial.println(v1);

  
  Serial.println("LDR2: (pin 1)");
  Serial.println(v2);

  delay(1000);

}
