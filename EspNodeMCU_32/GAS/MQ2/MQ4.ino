#include <Wire.h>  //lib para comunicação i2c
#define MQ_analog 12//pino analógico do sensor de gás
#define MQ_dig 13    //pino digital do sensor de gás
int sensorThres = 900; // detecta butano
//int sensorThres = 1370; // detecta CO2

void setup() {
  pinMode(MQ_analog, INPUT);
  Serial.begin(115200);

}

void loop() {
  int analogSensor = analogRead(MQ_analog);
  int digitalSensor = digitalRead(MQ_dig);
  Serial.print("Pin A0 nivel de voltaje por ppmm: ");
  Serial.println(analogSensor);
  Serial.print("Pin D0 indicador de deteccion de gas: ");
  Serial.println(digitalSensor);
  if (analogSensor > sensorThres)
  {
    Serial.print("STATE : red\n");
  }
  else
  {
    Serial.print("STATE : green\n");
  }
  delay(1000);



}
