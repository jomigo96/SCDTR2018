
const int ledPin = 7;

int value=0;
int i;
char buffer[10
];

void setup(){

  //pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop(){

  i=0;
  while(Serial.available()){
    buffer[i]=Serial.read();
    i++;
    
  }
  if(i>0){
    buffer[i]=0;
    value=atoi(buffer);
  }
  
  
  Serial.println(value);
  
  
  analogWrite(ledPin, value);
  delay(100);



}
