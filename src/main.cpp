
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

// If using the breakout with SPI, define the pins for SPI communication.
#define PN532_SCK  (18)
#define PN532_MOSI (23)
#define PN532_SS   (45)
#define PN532_MISO (19)

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (32)
#define PN532_RESET (4)  // Not connected by default on the NFC Shield

// Uncomment just _one_ line below depending on how your breakout or shield
// is connected to the Arduino:

// Use this line for a breakout with a SPI connection:
// Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

// Use this line for a breakout with a hardware SPI connection.  Note that
// the PN532 SCK, MOSI, and MISO pins need to be connected to the Arduino's
// hardware SPI SCK, MOSI, and MISO pins.  On an Arduino Uno these are
// SCK = 13, MOSI = 11, MISO = 12.  The SS line can be any digital IO pin.
// Adafruit_PN532 nfc(PN532_SS);

// Or use this line for a breakout or shield with an I2C connection:
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

volatile bool connected = false;


bool nfcConnect() {
  
  nfc.begin();

  // Connected, show version
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata)
  {
    Serial.println("PN53x card not found!");
    return false;
  }

  //port
  Serial.print("Found chip PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware version: "); Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);

  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0x0A);

  // configure board to read RFID tags
  nfc.SAMConfig();

  Serial.println("Waiting for card (ISO14443A Mifare)...");
  return true;
}


void handleCardDetected() {
  
  bool success;
  // Buffer to store the UID
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  // UID size (4 or 7 bytes depending on card type)
  uint8_t uidLength;

  while (!connected) {
    connected = nfcConnect();
  }

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);

  // If the card is detected, print the UID
  if (success) {
    Serial.println("Card Read");
    Serial.print("Size of UID: "); Serial.print(uidLength, DEC);
    Serial.println(" bytes");
    Serial.print("UID: ");
    for (uint8_t i = 0; i < uidLength; i++)
    {
      Serial.print(" 0x"); Serial.print(uid[i], HEX);
    }
    Serial.println("\n\n");
        
    delay(1000);
  } else {
    // PN532 probably timed out waiting for a card
    Serial.println("Timed out waiting for a card");
  }
}


void setup(void)
{
  Serial.begin(115200);
  Serial.println("*** Testing Module PN532 NFC RFID ***");
}


void loop(void)
{
  handleCardDetected();
  delay(750);
}

