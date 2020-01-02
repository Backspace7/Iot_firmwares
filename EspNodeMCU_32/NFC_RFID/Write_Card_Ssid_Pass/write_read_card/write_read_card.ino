
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
      
    
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 8, 0, keya);
    
      if (success)
      {
        Serial.println("Sector 1 (Blocks 8..11) has been authenticated");
         
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
       
        
        substr1.getBytes(data, 16);
        substr2.getBytes(data1, 16);
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