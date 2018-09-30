const int ledPin = 3;
const int buttonPin = 35;  //Works on MEGA

int led_value = 255;
int ledState = 0;

void setup() {
  
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
}

void loop() {

  int i;
  char buf[10];
  char s[30];

  i=0;
  while(Serial.available()){
    buf[i++]=Serial.read();
  }
  if(i>0){
    buf[i]=0;
    led_value = atoi(buf);
    led_value = (led_value > 255) ? 255 : led_value;
    led_value = (led_value < 0) ? 0 : led_value;
    sprintf(s, "Led dut cycle is now %d", led_value);
    Serial.println(s);
  }

  ledState = digitalRead(buttonPin);
  Serial.println(ledState);

  if(ledState)
    analogWrite(ledPin, led_value);
  else
    analogWrite(ledPin, 0);

  delay(100);
}
