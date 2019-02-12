
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "EEPROM.h"
#include "DallasTemperature.h"

#define PN532_SCK  (18)
#define PN532_MOSI (23)
#define PN532_SS   (5)
#define PN532_MISO (19)
#define ONE_WIRE_BUS 21

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
StaticJsonBuffer<200> jsonBuffer;

uint8_t sensor1[8] = { 0x28, 0x9C, 0xB4, 0x77, 0x91, 0x11, 0x2, 0xBB };
uint8_t sensor2[8] = { 0x28, 0xF1, 0x4E, 0x77, 0x91, 0xB, 0x2, 0x22 };
uint8_t keya[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
const char* ssid = "prrt";
const char* password =  "passwordmala";
const char* new_ssid ;
const char* new_password;
String new_ssid_str;
String new_password_str;
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
int EEPROM_SIZE = 48;

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

void scanNetworks() {
 
     int numberOfNetworks = WiFi.scanNetworks();
   
     Serial.print("Number of networks found: ");
     Serial.println(numberOfNetworks);
   
     for (int i = 0; i < numberOfNetworks; i++) {
   
         Serial.print("Network name: ");
         Serial.println(WiFi.SSID(i));
     
         Serial.print("Signal strength: ");
         Serial.println(WiFi.RSSI(i));
     
         Serial.print("MAC address: ");
         Serial.println(WiFi.BSSIDstr(i));
     
         Serial.print("Encryption type: ");
         String encryptionTypeDescription = translateEncryptionType(WiFi.encryptionType(i));
         Serial.println(encryptionTypeDescription);
         Serial.println("-----------------------");

     }
}

void connectToNetwork() {

     try_connect = 0;
      
     Serial.println("Establishing connection to WiFi..");
     WiFi.begin(ssid, password);
     
     while (WiFi.status() != WL_CONNECTED &&  try_connect < 10) {
      
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
        Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
        Serial.print("  UID Value: ");
        nfc.PrintHex(uid, uidLength);
        Serial.println("");
        
        if (uidLength == 4){
          
            Serial.println("Seems to be a Mifare Classic card (4 byte UID)");
            Serial.println("Trying to authenticate block 8 with default KEYA value");
            
            success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 8, 0, keya);
            
            if (success){
              
               Serial.println("Sector 1 (Blocks 8..11) has been authenticated");
               
               memset(data_aux1, 0, sizeof(data_aux1));
               memset(data_aux2, 0, sizeof(data_aux2));
               memset(data_aux3, 0, sizeof(data_aux3));
               success =  nfc.mifareclassic_ReadDataBlock( 8, data_aux1);
                          nfc.mifareclassic_ReadDataBlock( 9, data_aux2);
                          nfc.mifareclassic_ReadDataBlock( 10, data_aux3);
                          
               if(success){
                
                  nfc.PrintHexChar(data_aux1, 16);
                  nfc.PrintHexChar(data_aux2, 16);
                  nfc.PrintHexChar(data_aux3, 16);
      
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
               /*EEPROM.writeString(addr, new_ssid);
               EEPROM.writeString(addr+10, new_password);
               EEPROM.commit();*/
               /*new_ssid_str = EEPROM.readString(addr);
               new_password_str = EEPROM.readString(addr+10);
               ssid = new_ssid_str.c_str();
               password = new_password_str.c_str();*/
               ssid = new_ssid;
               password = new_password;
               
               Serial.println("ssid and password has been changued");
               Serial.print(ssid);Serial.print(" ");Serial.println(password);
               Serial.println(str);
               
               jsonBuffer.clear();
               
               connectToNetwork();
               
              }
              else{
                
                 Serial.println("Ooops ... authentication failed: Try another key?");
                 
              }
          }
      }
}
void printTemperature(DeviceAddress deviceAddress){
  
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
     ssid = new_ssid_str.c_str();
     password = new_password_str.c_str();
    
     Serial.println(ssid);
     Serial.println(password);
    
     Serial.println("Hello!");
    
     scanNetworks();
     connectToNetwork();

     sensors.begin();
     nfc.begin();
     
     uint32_t versiondata = nfc.getFirmwareVersion();
     if (! versiondata) {
      
        Serial.print("Didn't find PN53x board");
        while (1); // halt
    
     }
     
     Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
     Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
     Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
    
     nfc.SAMConfig();
  
}


void loop(void) {
  Serial.println(WiFi.status());
  
  if(WiFi.status()!= WL_CONNECTED){
    
     connectToNetwork();

  }
  delay(3000);
  //configureEsp32Network();
  sensors.requestTemperatures();
  
  Serial.print("Sensor 1: ");
  printTemperature(sensor1);
  
  Serial.print("Sensor 2: ");
  printTemperature(sensor2);
  delay(3000);

}
