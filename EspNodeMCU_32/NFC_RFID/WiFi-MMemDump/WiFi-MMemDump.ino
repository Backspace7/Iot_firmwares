#include <WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#define PN532_SCK  (18)
#define PN532_MOSI (23)
#define PN532_SS   (5)
#define PN532_MISO (19)

Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
#if defined(ARDUINO_ARCH_SAMD)
   #define Serial SerialUSB
#endif

const char* ssid = "REsDF";
const char* password =  "Capos6c919c";
unsigned long check_wifi = 30000;

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
  WiFi.begin(ssid, password);
  unsigned long try_wifi = 0;
  while (WiFi.status() != WL_CONNECTED && try_wifi < 5) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
    try_wifi = try_wifi+1;
  }
  Serial.println("Connected to network");
}
 
void setup() {
 
  Serial.begin(115200);
  scanNetworks();
  connectToNetwork();
 
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
 
  //WiFi.disconnect(true);
  Serial.println(WiFi.localIP());

  Serial.println("Looking for PN532...");
  
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  // configure board to read RFID tags
  nfc.SAMConfig();

  Serial.println("Waiting for an ISO14443A Card ...");

  
}
void loop() {
  
  if ((WiFi.status() != WL_CONNECTED) && (millis() > check_wifi)) {
      Serial.println("Reconnecting to WiFi...");
      WiFi.disconnect();
      ssid = "REDF";
      password="Capos6919c";
      WiFi.begin(ssid, password);
      check_wifi = millis() + 30000;
  }
  Serial.println(WiFi.localIP());
  uint8_t success;                          // Flag to check if there was an error with the PN532
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  uint8_t currentblock;                     // Counter to keep track of which block we're on
  bool authenticated = false;               // Flag to indicate if the sector is authenticated
  uint8_t data[16];                         // Array to store block data during reads
   uint8_t keyuniversal[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

  
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");

    if (uidLength == 4)
    {
      Serial.println("Seems to be a Mifare Classic card (4 byte UID)");
      for (currentblock = 0; currentblock < 20; currentblock++)
      {
        if (nfc.mifareclassic_IsFirstBlock(currentblock)) authenticated = false;

        if (!authenticated)
        {
          Serial.print("------------------------Sector ");Serial.print(currentblock/4, DEC);Serial.println("-------------------------");
          if (currentblock == 0)
          {
              success = nfc.mifareclassic_AuthenticateBlock (uid, uidLength, currentblock, 1, keyuniversal);
          }
          else
          {
              success = nfc.mifareclassic_AuthenticateBlock (uid, uidLength, currentblock, 1, keyuniversal);
          }
          if (success)
          {
            authenticated = true;
          }
          else
          {
            Serial.println("Authentication error");
          }
        }
        // If we're still not authenticated just skip the block
        if (!authenticated)
        {
          Serial.print("Block ");Serial.print(currentblock, DEC);Serial.println(" unable to authenticate");
        }
        else
        {
          success = nfc.mifareclassic_ReadDataBlock(currentblock, data);
          if (success)
          {
            // Read successful
            Serial.print("Block ");Serial.print(currentblock, DEC);
            if (currentblock < 10)
            {
              Serial.print("  ");
            }
            else
            {
              Serial.print(" ");
            }
            // Dump the raw data
            nfc.PrintHexChar(data, 16);
          }
          else
          {
            // Oops ... something happened
            Serial.print("Block ");Serial.print(currentblock, DEC);
            Serial.println(" unable to read this block");
          }
        }
      }
    }
    else
    {
      Serial.println("Ooops ... this doesn't seem to be a Mifare Classic card!");
    }
  }
  Serial.println("\n\nSend a character to run the mem dumper again!");
  Serial.flush();
  while (!Serial.available());
  while (Serial.available()) {
  Serial.read();
  }
  Serial.flush();
  
  delay(5000);

  }
