/*
   EEPROM Write
   Stores random values into the EEPROM.
   These values will stay in the EEPROM when the board is
   turned off and may be retrieved later by another sketch.
*/

#include "EEPROM.h"

// the current address in the EEPROM (i.e. which byte
// we're going to write to next)
int addr = 0;
int i =0;
#define EEPROM_SIZE 48
const char* ssid = "REDF";
const char* password = "Capos6919c";
String ssid_str = "REDF";
String password_str = "Capos6919c";
void setup()
{
  Serial.begin(115200);
  Serial.println("start...");
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM"); delay(1000000);
  }
  EEPROM.writeString(addr, ssid);
  EEPROM.writeString(addr+10, password);
  EEPROM.commit();
  Serial.println("writing random n. in memory");
}

void loop()
{
  
  if(i< 2){
    Serial.print(" ");
    Serial.print(EEPROM_SIZE);
    Serial.println(" bytes written on Flash . Values are:");
    ssid_str = EEPROM.readString(addr);
    password_str = EEPROM.readString(addr+10);
    Serial.print(ssid); Serial.print(" ");Serial.print(password); Serial.print(" ");
    Serial.println(); Serial.println("----------------------------------");
    Serial.print(EEPROM.readString(addr+1)); Serial.println(" ");Serial.print(EEPROM.readString(addr+10));
  }
  i = i+1;
  delay(500);
}
