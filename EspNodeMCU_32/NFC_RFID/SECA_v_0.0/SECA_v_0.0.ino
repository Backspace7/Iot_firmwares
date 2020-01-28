
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "EEPROM.h"
#include "DallasTemperature.h"
#include <HTTPClient.h>
#include <SFE_BMP180.h>
#include "DHT.h"

#define PN532_SCK  (18)
#define PN532_MOSI (23)
#define PN532_SS   (5)
#define PN532_MISO (19)
#define ONE_WIRE_BUS 4
#define DHTPIN 17
#define DHTTYPE DHT22
#define MQ_analog 36
#define MQ_dig 13
#define LEDPIN 2
#define LEDPIN2 12
#define LEDPIN3 27
#define WIFIPIN 16
#define PushButton 25
#define PushButton2 26
#define PushButton3 35
#define lightPin 32

DHT dht(DHTPIN, DHTTYPE);
SFE_BMP180 bmp180;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
StaticJsonBuffer<200> jsonBuffer;

bool button_status=false;
int lightCal;
int lightVal;
const int sensorThres = 900;
uint8_t sensor1[8] = { 0x28, 0x9C, 0xB4, 0x77, 0x91, 0x11, 0x2, 0xBB };
uint8_t sensor2[8] = { 0x28, 0xF1, 0x4E, 0x77, 0x91, 0xB, 0x2, 0x22 };
uint8_t sensor3[8] = { 0x28, 0x9D, 0xDE, 0x77, 0x91, 0xE, 0x2, 0xD4 };
uint8_t keya[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
const char* ssid = "prrt";
const char* password =  "passwordmala";
const char* url =  "midominio.cl/recuros";
const char* new_ssid ;
const char* new_password;
const char* new_url;
String new_ssid_str;
String new_password_str;
String new_url_str;
unsigned long check_wifi = 30000;
String str = " ";
String str1 = " ";
String str2 = " ";
String str3 = " ";
uint8_t data_aux1[16];
uint8_t data_aux2[16];
uint8_t data_aux3[16];
uint8_t success;
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  
uint8_t uidLength;              
unsigned long try_connect = 0;
int addr = 0;
int EEPROM_SIZE = 84;

String translateEncryptionType(wifi_auth_mode_t encryptionType) {
       switch (encryptionType) {
         case (WIFI_AUTH_OPEN):
           return "Open";
         case (WIFI_AUTH_WEP):
           return "WEP";
         case (WIFI_AUTH_WPA_PSK):
           return "WPA_PSK";
         case (WIFI_AUTH_WPA2_PSK):
           return "WPA2_PSK";
         case (WIFI_AUTH_WPA_WPA2_PSK):
           return "WPA_WPA2_PSK";
         case (WIFI_AUTH_WPA2_ENTERPRISE):
           return "WPA2_ENTERPRISE";
      }
}

void connectToNetwork() {

     try_connect = 0;
     Serial.println("Establishing connection to WiFi..");
     WiFi.begin(ssid, password);
     while (WiFi.status() != WL_CONNECTED &&  try_connect < 5) {   
           delay(500);
           Serial.println(".");
           if(WiFi.status() == WL_CONNECTED ){
              Serial.println("Connected to network");
              Serial.println(WiFi.macAddress());
              Serial.println(WiFi.localIP());
           }
           try_connect = try_connect + 1;        
     }
}
void configureEsp32Network(){
     str = " ";
     str1 = " ";
     str2 = " ";
     str3 = " ";
     new_ssid = " ";
     new_password = " ";
     Serial.println("Waiting for an ISO14443A Card ...");
     success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
     if (success) {
        Serial.println("Found an ISO14443A card");
        Serial.println("");
        if (uidLength == 4){
            Serial.println("Trying to authenticate ");
            success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 8, 0, keya);
            if (success){
               Serial.println("has been authenticated");
               memset(data_aux1, 0, sizeof(data_aux1));
               memset(data_aux2, 0, sizeof(data_aux2));
               memset(data_aux3, 0, sizeof(data_aux3));
               success =  nfc.mifareclassic_ReadDataBlock( 8, data_aux1);
                          nfc.mifareclassic_ReadDataBlock( 9, data_aux2);
                          nfc.mifareclassic_ReadDataBlock( 10, data_aux3);     
               if(success){
                  Serial.println("");
                  str1 = (char*)data_aux1;
                  str2 = (char*)data_aux2;
                  str3 = (char*)data_aux3;
                  str = str1 +str2 +str3;     
                  delay(1000);                  
               } 
               else{                
                  Serial.println("Ooops ... unable to read the requested block.  Try another key?");                  
               }               
               JsonObject& root = jsonBuffer.parseObject(str);
               new_ssid = root["ssid"];
               new_password = root["password"];
               EEPROM.writeString(addr, new_ssid);
               EEPROM.writeString(addr+10, new_password);
               EEPROM.commit();
               new_ssid_str = EEPROM.readString(addr);// Descomentar cuando se desee cambiar la password en la EMPROM
               new_password_str = EEPROM.readString(addr+10);
               ssid = new_ssid_str.c_str();
               password = new_password_str.c_str();
              // ssid = new_ssid;
              // password = new_password;               
               Serial.println("ssid and password has been changued");
               Serial.print(ssid);Serial.print(" ");Serial.println(password);
              // Serial.println(str);               
               jsonBuffer.clear();               
               connectToNetwork();               
              }
              else{                
                 Serial.println("Ooops ... authentication failed: Try another key?");                 
              }
          }
      }
}
void configure_url(){
      str = " ";
     str1 = " ";
     str2 = " ";
     str3 = " ";
     new_url=" ";     
     Serial.println("Waiting for an ISO14443A Card ...");
     success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);      
     if (success) {      
        Serial.println("Found an ISO14443A card");
        Serial.println("");        
        if (uidLength == 4){          
            Serial.println("Trying to authenticate ");
            success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 12, 0, keya);            
            if (success){              
               Serial.println("has been authenticated");               
               memset(data_aux1, 0, sizeof(data_aux1));
               memset(data_aux2, 0, sizeof(data_aux2));
               memset(data_aux3, 0, sizeof(data_aux3));
               success =  nfc.mifareclassic_ReadDataBlock( 12, data_aux1);
                          nfc.mifareclassic_ReadDataBlock( 13, data_aux2);
                          nfc.mifareclassic_ReadDataBlock( 14, data_aux3);                          
               if(success){      
                  Serial.println("");
                  str1 = (char*)data_aux1;
                  str2 = (char*)data_aux2;
                  str3 = (char*)data_aux3;
                  str = str1 +str2 +str3;     
                  delay(1000);                  
               } 
               else{                
                  Serial.println("Ooops ... unable to read the requested block.  Try another key?");                  
               }               
               JsonObject& root = jsonBuffer.parseObject(str);
               new_url = root["url"];
               EEPROM.writeString(addr+25, new_url);
               EEPROM.commit();
               new_url_str = EEPROM.readString(addr+25);// Descomentar cuando se desee cambiar la password en la EMPROM
               url = new_url_str.c_str();               
               Serial.println("url has been changued");
               Serial.print(url);Serial.print(" ");
              // Serial.println(str);               
               jsonBuffer.clear();               
               connectToNetwork();               
              }
              else{                
                 Serial.println("Ooops ... authentication failed: Try another key?");                 
              }
          }
      }  
}
void bmp_sensor(){
  char status;
  double T,P;
  status = bmp180.startTemperature();
  if (status != 0){   
    delay(status); //Pausa para que finalice la lectura
    status = bmp180.getTemperature(T); //Obtener la temperatura
    if (status != 0){
      status = bmp180.startPressure(3); //Inicio lectura de presión
      if (status != 0){        
        delay(status);//Pausa para que finalice la lectura        
        status = bmp180.getPressure(P,T); //Obtenemos la presión
        if (status != 0){                  
          Serial.print("Temperatura: ");
          Serial.print(T,2);
          Serial.print(" *C , ");
          Serial.print("Presion: ");
          Serial.print(P,2);
          Serial.println(" mb");          
        }      
      }      
    }   
  }   
}

void dht22_sensor(){
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
void printTemperature(DeviceAddress deviceAddress){// cambiar el print por return para ponerlo en el json y enviar los dtos a la api  
     float tempC = sensors.getTempC(deviceAddress);
     Serial.print(tempC);
     Serial.print("°C  |  ");
     Serial.print(DallasTemperature::toFahrenheit(tempC));
     Serial.println("°F");  
}

void ds18b20_sensors(){
   sensors.requestTemperatures();
   Serial.print("Sensor 1: ");
   printTemperature(sensor1);
   Serial.print("Sensor 2: ");
   printTemperature(sensor2);  
}
void mq4_sensor(){
 int analogSensor = analogRead(MQ_analog);
  int digitalSensor = digitalRead(MQ_dig);
  Serial.print("Pin A0 nivel de voltaje por ppmm: ");
  Serial.println(analogSensor* (5.0 / 1023.0));
  Serial.print("Pin D0 indicador de deteccion de gas: ");
  Serial.println(digitalSensor);
  if (analogSensor > sensorThres){
    Serial.print("STATE : red\n");
  }
  else{
    Serial.print("STATE : green\n");
  }  
}

void light_sensor(){
   //Take a reading using analogRead() on sensor pin and store it in lightVal
  lightVal = analogRead(lightPin);


  //if lightVal is less than our initial reading (lightCal) minus 50 it is dark and
  //turn pin 9 HIGH. The (-50) part of the statement sets the sensitivity. The smaller
  //the number the more sensitive the circuit will be to variances in light.
  if (lightVal < lightCal - 50)
  {
    Serial.println("no luz");
  }

  //else, it is bright, turn pin 9 LOW
  else
  {
    Serial.println("luz");
  }  
}
void setup(void) {
     
     Serial.begin(115200);    
     if (!EEPROM.begin(EEPROM_SIZE)){      
         Serial.println("failed to initialise EEPROM"); delay(1000000);    
     }
     lightCal = analogRead(lightPin);
     new_ssid_str = EEPROM.readString(addr);
     new_password_str = EEPROM.readString(addr+10);
     new_url_str = EEPROM.readString(addr+25);
     url = new_url_str.c_str();
     ssid = new_ssid_str.c_str();
     password = new_password_str.c_str();    
     Serial.println(ssid);
     Serial.println(password);
     Serial.println(url);
     Serial.println("Hello!");    
     connectToNetwork();
     pinMode(MQ_analog, INPUT);
     sensors.begin();
     dht.begin();
     if (bmp180.begin())
       Serial.println("BMP180 iniciado correctamenten");
     else{
       Serial.println("Error al iniciar el BMP180");
       while(1); // bucle infinito
     }
     nfc.begin();     
     uint32_t versiondata = nfc.getFirmwareVersion();
     if (! versiondata) {      
        Serial.print("Didn't find PN53x board");
        while (1); // halt    
     }     
     Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);     
     nfc.SAMConfig();
     pinMode(LEDPIN, OUTPUT);
     pinMode(LEDPIN2, OUTPUT);
     pinMode(LEDPIN3, OUTPUT);
     pinMode(WIFIPIN, OUTPUT);
     pinMode(PushButton, INPUT);     
     pinMode(PushButton2, INPUT);
     pinMode(PushButton3, INPUT);     
}

void send_data_to_API(String payload){
   HTTPClient http; 
   http.begin("http://jsonplaceholder.typicode.com/posts");  //Specify destination for HTTP request
   http.addHeader("Content-Type", "application/json");             //Specify content-type header  
  int httpResponseCode = http.POST(payload);   //Send the actual POST request 
   if(httpResponseCode>0){ 
    String response = http.getString();                       //Get the response to the request 
    Serial.println(httpResponseCode);   //Print return code
    Serial.println(response);           //Print request answer 
   }else{ 
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode); 
   } 
   http.end();  //Free resources  
}
void loop(void) {  
  
  //String payload = "{\"body\":\"bar\",\"title\":\"foo\",\"userId\":1}";  
  if(WiFi.status()!= WL_CONNECTED){
     digitalWrite(WIFIPIN,LOW);
     connectToNetwork();
     delay(3000);
  }else{
   Serial.println("Status ok ----");
   Serial.println(WiFi.localIP());
   digitalWrite(WIFIPIN, HIGH);
   bmp_sensor();
   dht22_sensor();
   light_sensor();
   ds18b20_sensors();
   mq4_sensor();   
   //send_data_to_API(payload)   
  }
  delay(3000);
  int Push_button_state = digitalRead(PushButton);
  int Push_button_state2 = digitalRead(PushButton2);
  int Push_button_state3 = digitalRead(PushButton3);
  if ( Push_button_state == LOW ){ 
    digitalWrite(LEDPIN, HIGH); 
    configureEsp32Network();
  }
  else{
    digitalWrite(LEDPIN, LOW); 
    //Serial.println(Push_button_state);
  }  
  if ( Push_button_state2 == LOW ){ 
    digitalWrite(LEDPIN2, HIGH); 
    configure_url();
  }
  else{
    digitalWrite(LEDPIN2, LOW); 
    //Serial.println(Push_button_state);
   }

  if ( Push_button_state3 == LOW ){ 
    
    digitalWrite(LEDPIN3, HIGH); 
    delay(2000);
    ESP.restart();
  }
  else{
    digitalWrite(LEDPIN3, LOW); 
   }  
   
  
}
