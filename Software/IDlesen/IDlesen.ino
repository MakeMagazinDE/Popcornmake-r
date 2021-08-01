// IDlesen
// Auslesen und Anzeige von RFID-Tags über den seriellen Monitor

#include <SPI.h> 
#include <MFRC522.h>
#define SS_PIN 10 // SDA an Pin 10
#define RST_PIN 9 // RST an Pin 9

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup()
{
Serial.begin(115200);
SPI.begin();
mfrc522.PCD_Init();
}

void loop() 
{

if ( ! mfrc522.PICC_IsNewCardPresent()) // Ohne RFID-Tag
{
return; // ...beginnt die Loop-Schleife erneut
}

if ( ! mfrc522.PICC_ReadCardSerial()) // Ohne RFID-Sensor...
{
return; // ebenfalls
}

Serial.print("RFID-TAG erkannt! Die ID lautet: ");

for (byte i = 0; i < mfrc522.uid.size; i++)
{
Serial.print(mfrc522.uid.uidByte[i], HEX); //ID byteweise einlesen und hexadezimal ausgeben
}
Serial.println();
}
