//Conecte el Pin_1(Vcc) del DHT22 al Pin de 5V del Arduino. 
//Conecte el Pin_4(GND) del DHT22 al Pin GND del Arduino. 
//Conecte el Pin_2(SDA) del DHT22 al Pin D2 del Arduino.

#include "DHT.h"
#define DHTPIN 17  
// Se indica con cual sensor se va a trabajar DHT 22 
#define DHTTYPE DHT22   

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200); 
  dht.begin();
}
void loop() {
  // Se esperan 2 segundos para tomar mediciones.
  delay(2000);
  // Se lee la humedad
  float h = dht.readHumidity();
  // Lee la temperatura en grados Celsius (°C)
  float t = dht.readTemperature();
  // Comprueba si ocurrió un fallo en la lectura y lo intenta de nuevo.
  if (isnan(h) || isnan(t)) {
    Serial.println("Error al leer el sensor DHT!");
    return;
  }
  
  Serial.print("Humedad: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperatura: "); 
  Serial.print(t);
  Serial.println(" *C ");
}