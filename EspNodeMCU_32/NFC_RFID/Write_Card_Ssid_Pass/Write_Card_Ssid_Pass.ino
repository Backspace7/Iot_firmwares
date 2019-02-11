      /*uint8_t keya[6] = { 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5 };
      //uint8_t keya[6] = { 0xd3, 0xf7, 0xd3, 0xf7, 0xd3, 0xf7 };
      //uint8_t keya[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
      //uint8_t keya[6] = { 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5 };
      //uint8_t keya[6] = { 0x4d, 0x3a, 0x99, 0xc3, 0x51, 0xdd };
      //uint8_t keya[6] = { 0x1a, 0x98, 0x2c, 0x7e, 0x45, 0x9a };
      //uint8_t keya[6] = { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
      //uint8_t keya[6] = { 0x71, 0x4c, 0x5c, 0x88, 0x6e, 0x97 };
      //uint8_t keya[6] = { 0x58, 0x7e, 0xe5, 0xf9, 0x35, 0x0f };
      //uint8_t keya[6] = { 0xa0, 0x47, 0x8c, 0xc3, 0x90, 0x91 };
      //uint8_t keya[6] = { 0x53, 0x3c, 0xb6, 0xc7, 0x23, 0xf6 };
      //uint8_t keya[6] = { 0x8f, 0xd0, 0xa4, 0xf2, 0x56, 0xe9 };*/
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
 const char* new_ssid = " ";
  const char* new_password = " ";
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

void loop(void) {
   
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  
  uint8_t uidLength;                        
  new_ssid = "";
  new_password="";
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
    
    if (uidLength == 4)// Ampliarlo a uid == 7 misma accion
    {
      Serial.println("Seems to be a Mifare Classic card (4 byte UID)");
      Serial.println("Trying to authenticate block 8 with default KEYA value");
      uint8_t keya[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 8, 0, keya);
    
      if (success)
      {
        Serial.println("Sector 1 (Blocks 8..11) has been authenticated");
         String str = " ";
         String str1 = " ";
         String str2 = " ";
         uint8_t data_aux1[16];
         uint8_t data_aux2[16];
         uint8_t data_aux3[16];
         success =  nfc.mifareclassic_ReadDataBlock( 8, data_aux1);
                    nfc.mifareclassic_ReadDataBlock( 9, data_aux2);
                    nfc.mifareclassic_ReadDataBlock( 10, data_aux3);
         if(success){
            nfc.PrintHexChar(data_aux1, 16);
            nfc.PrintHexChar(data_aux2, 16);
            nfc.PrintHexChar(data_aux3, 16);
             
            Serial.println("");
            str = (char*)data_aux1;
            str1 = (char*)data_aux2;
            str2 = (char*)data_aux3;
            str = str +str1 +str2;
            Serial.println(str);
            JsonObject& root = jsonBuffer.parseObject(str);
            new_ssid = root["ssid"];
            new_password = root["password"];
            Serial.print("Actual ssid :");Serial.println(new_ssid);
            Serial.print("Actual Password :");Serial.println(new_password);
            jsonBuffer.clear();
         } 
         else{
           Serial.println("Ooops ... unable to read the requested block.  Try another key?");
         }
          String pass_ = " ";
          String ssid_ = " ";
          
          Serial.println("Enter your Ssid.");  
          while(Serial.available()==0) { }  
          delay(500);
          
          ssid_=Serial.readString();
          ssid_.trim();    
          Serial.println("SSID : " +ssid_);  
          delay(500);

          Serial.println("Enter your Password.");  
          while(Serial.available()==0){ }
          delay(500);
          
          pass_=Serial.readString();
          pass_.trim();   
          Serial.println("PASSWORD : " +pass_);  
          delay(500);

          JsonObject& jsonarray = jsonBuffer.createObject();
          jsonarray["ssid"] = ssid_;
          jsonarray["password"] = pass_;
          Serial.println(" JSON ");
          String json_str = " ";
          jsonarray.printTo(json_str);
          Serial.println(json_str);
          jsonBuffer.clear();
         String substr1 = json_str.substring(1,16);
         String substr2 = json_str.substring(16,31);
         String substr3 = json_str.substring(31);
         Serial.println(substr1);
         Serial.println(substr2);
         Serial.println(substr3);
         /*for(int i = 0; i < 16 ; i++){
            Serial.print(" 0x");Serial.print(buff[i], HEX);
          }
          Serial.print(" ");
          for(int i = 0; i < 16 ; i++){
            Serial.write(buff[i]);
          }*/
       
        uint8_t data[16];
        substr1.getBytes(data, 16);
        uint8_t data1[16];
        substr2.getBytes(data1, 16);
        uint8_t data2[16];
        substr3.getBytes(data2, 16);
        success = nfc.mifareclassic_WriteDataBlock (8, data);
        success = nfc.mifareclassic_WriteDataBlock (9, data1);
        success = nfc.mifareclassic_WriteDataBlock (10, data2);

        /*memcpy(data, (const uint8_t[]) {'{','"','s','s','i','d','"',':','"','R','E','D','F','"',',','"'}, sizeof data);
        success = nfc.mifareclassic_WriteDataBlock (8, data);
        memcpy(data1, (const uint8_t[]){'p','a','s','s','w','o','r','d','"',':','"','C','a','p','o','s'}, sizeof data1);
        success = nfc.mifareclassic_WriteDataBlock (9, data1);
        memcpy(data2, (const uint8_t[]){'6','9','1','9','c','"','}',0,0,0,0,0,0,0,0,0}, sizeof data2);
        success = nfc.mifareclassic_WriteDataBlock (10, data2);*/
        
  
       
        /*Serial.println("ssid and password has been changued");
        Serial.print("New ssid :");Serial.println(ssid);
        Serial.println("New Password :");Serial.println(password);*/

      }
      else
      {
        Serial.println("Ooops ... authentication failed: Try another key?");
      }
      
    }
    delay(3000);

  }
  
}
