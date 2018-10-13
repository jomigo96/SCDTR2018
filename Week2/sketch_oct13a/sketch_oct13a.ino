const int sensor_pin = 0;
int sensor_value;
float v;
float R;
float L;

const float b = 5.8515;
const float m = -0.9355;
const float Raux = 100000;

void setup() {

  Serial.begin(9600);
}

void loop() {

  sensor_value = analogRead(sensor_pin);

  v = sensor_value*5.0/1.0230;

  R = (5-v/1000.0)/(v/1000.0/Raux);

  L = pow(10, (log10(R)-b)/m );

  Serial.println("luminosity:");
  Serial.println(L);

  delay(1000);

}
