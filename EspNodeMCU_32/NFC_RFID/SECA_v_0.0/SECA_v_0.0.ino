
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "EEPROM.h"
#include "DallasTemperature.h"
#include <HTTPClient.h>

#define PN532_SCK  (18)
#define PN532_MOSI (23)
#define PN532_SS   (5)
#define PN532_MISO (19)
#define ONE_WIRE_BUS 21

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
StaticJsonBuffer<200> jsonBuffer;
bool button_status=false;
const int LEDPIN = 22; 
const int WIFIPIN=4;
const int PushButton =15;
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
void printTemperature(DeviceAddress deviceAddress){// cambiar el print por return para ponerlo en el json y enviar los dtos a la api
  
     float tempC = sensors.getTempC(deviceAddress);
     Serial.print(tempC);
     Serial.print("°C  |  ");
     Serial.print(DallasTemperature::toFahrenheit(tempC));
     Serial.println("°F");
  
}
void setup(void) {
  
     Serial.begin(115200);
    
     if (!EEPROM.begin(EEPROM_SIZE)){
      
         Serial.println("failed to initialise EEPROM"); delay(1000000);
    
     }
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

     sensors.begin();
     nfc.begin();
     
     uint32_t versiondata = nfc.getFirmwareVersion();
     if (! versiondata) {
      
        Serial.print("Didn't find PN53x board");
        while (1); // halt
    
     }
     
     Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
    
     nfc.SAMConfig();
     pinMode(LEDPIN, OUTPUT);
     pinMode(WIFIPIN, OUTPUT);
     pinMode(PushButton, INPUT);
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
  //sensors.requestTemperatures();
  //Serial.println("Status ok ----");
  //Serial.print("Sensor 1: ");
  //printTemperature(sensor1);
  
  //Serial.print("Sensor 2: ");
  //printTemperature(sensor2);
  //String payload = "{\"body\":\"bar\",\"title\":\"foo\",\"userId\":1}";
  
  if(WiFi.status()!= WL_CONNECTED){
     digitalWrite(WIFIPIN,LOW);
     connectToNetwork();
     delay(3000);
  }else{
   Serial.println("Status ok ----");
   Serial.println(WiFi.localIP());
   digitalWrite(WIFIPIN, HIGH); 
   //send_data_to_API(payload)
   
  }
  delay(3000);
  int Push_button_state = digitalRead(PushButton);
  if ( Push_button_state == LOW ){ 
    digitalWrite(LEDPIN, HIGH); 
    configureEsp32Network();
  }
  else{
    digitalWrite(LEDPIN, LOW); 
    digitalWrite(LEDPIN, LOW);
    //Serial.println(Push_button_state);
   }
  
  configure_url();
  


}
