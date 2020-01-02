
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <ArduinoJson.h>
#define PN532_SCK  (18)
#define PN532_MOSI (23)
#define PN532_SS   (5)
#define PN532_MISO (19)

Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
StaticJsonBuffer<200> jsonBuffer;
const char* new_url = " ";
uint8_t success;
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  
uint8_t uidLength; 
uint8_t keya[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };                       
String str = " ";
String str1 = " ";
String str2 = " ";
uint8_t data_aux1[16];
uint8_t data_aux2[16];
uint8_t data_aux3[16];
String url_ = " ";

uint8_t data[16];
uint8_t data1[16];
uint8_t data2[16];


void setup(void) {
 
  Serial.begin(115200);
  Serial.println("Hello!");
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
  Serial.println("Waiting for an ISO14443A Card ...");
}

void read_card(){
  new_url = "";
  
  Serial.println("Waiting for an ISO14443A Card ...");
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) {
    Serial.println("Found an ISO14443A card");
    Serial.println("");
    
    if (uidLength == 4){
      Serial.println("Trying to authenticate ");
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 12, 0, keya);
      if (success){
         Serial.println("Has been authenticated");
         success =  nfc.mifareclassic_ReadDataBlock( 12, data_aux1);
                    nfc.mifareclassic_ReadDataBlock( 13, data_aux2);
                    nfc.mifareclassic_ReadDataBlock( 14, data_aux3);
         if(success){
            Serial.println("");
            str = (char*)data_aux1;
            str1 = (char*)data_aux2;
            str2 = (char*)data_aux3;
            str = str +str1 +str2;
            JsonObject& root = jsonBuffer.parseObject(str);
            new_url = root["url"];
            Serial.print("Actual ssid :");Serial.println(new_url);
            jsonBuffer.clear();
         } 
         else{
           Serial.println("Ooops ... unable to read the requested block.  Try another key?");
         }
      }
      else{
        Serial.println("Ooops ... authentication failed: Try another key?");
      }   
    }
    delay(3000);
  }
  
  
}


void write_card(){
  new_url = "";
  
  String json_str = " ";

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    Serial.println("Found an ISO14443A card");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
    
    if (uidLength == 4)// Ampliarlo a uid == 7 misma accion
    {
      Serial.println("Trying to authenticate ");
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 12, 0, keya);
    
      if (success)
      {
        Serial.println("has been authenticated");
         
         success =  nfc.mifareclassic_ReadDataBlock( 12, data_aux1);
                    nfc.mifareclassic_ReadDataBlock( 13, data_aux2);
                    nfc.mifareclassic_ReadDataBlock( 14, data_aux3);
         if(success){
             
            Serial.println("");
            str = (char*)data_aux1;
            str1 = (char*)data_aux2;
            str2 = (char*)data_aux3;
            str = str +str1 +str2;
            JsonObject& root = jsonBuffer.parseObject(str);
            new_url = root["url"];
            Serial.print("Actual url :");Serial.println(new_url);
           
            jsonBuffer.clear();
         } 
         else{
           Serial.println("Ooops ... unable to read the requested block.  Try another key?");
         }
          
          
          Serial.println("Enter your url.");  
          while(Serial.available()==0) { }  
          delay(500);
          
          url_=Serial.readString();
          url_.trim();    
          Serial.println("SSID : " +url_);  
          delay(500);

          JsonObject& jsonarray = jsonBuffer.createObject();
          jsonarray["url"] = url_;
         
          Serial.println(" JSON ");
          jsonarray.printTo(json_str);
          Serial.println(json_str);
          jsonBuffer.clear();
         String substr1 = json_str.substring(1,16);
         String substr2 = json_str.substring(16,31);
         String substr3 = json_str.substring(31,46);
         Serial.println(substr1);
         Serial.println(substr2);
         Serial.println(substr3);
        substr1.getBytes(data, 16);
        substr2.getBytes(data1, 16);
        substr3.getBytes(data2, 16);
        success = nfc.mifareclassic_WriteDataBlock (12, data);
        success = nfc.mifareclassic_WriteDataBlock (13, data1);
        success = nfc.mifareclassic_WriteDataBlock (14, data2);
      }
      else
      {
        Serial.println("Ooops ... authentication failed: Try another key?");
      }
      
    }
   

  }
  
  
}

void loop(void) {
  read_card();
   delay(3000);
  write_card();
  read_card();
  
}
