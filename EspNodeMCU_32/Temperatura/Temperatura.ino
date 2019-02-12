#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 21

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// Addresses of 3 DS18B20s
uint8_t sensor1[8] = { 0x28, 0x9C, 0xB4, 0x77, 0x91, 0x11, 0x2, 0xBB };
uint8_t sensor2[8] = { 0x28, 0xF1, 0x4E, 0x77, 0x91, 0xB, 0x2, 0x22 };


void setup(void)
{
  Serial.begin(115200);
  sensors.begin();
}

void loop(void)
{
  sensors.requestTemperatures();
  
  Serial.print("Sensor 1: ");
  printTemperature(sensor1);
  
  Serial.print("Sensor 2: ");
  printTemperature(sensor2);
  
  
  Serial.println();
  delay(1000);
}

void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print(tempC);
  Serial.print("°C  |  ");
  Serial.print(DallasTemperature::toFahrenheit(tempC));
  Serial.println("°F");
}
