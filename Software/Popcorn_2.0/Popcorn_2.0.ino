//Popcorn.ino

//Popcorn-Automat mit RFID-Kennung für Portionsgrößen

#include <SPI.h>
#include <MFRC522.h>


//Pin-Belegung RFID-Modul (RC522)
//    Modul   D1mini
//    Vcc     3.3V
//    RST     D9
//    GND     G
//    MISO    D12
//    MOSI    D11
//    SCK     D13
//    SDA/NSS D10
//    Dir     D5
//    Step    D2
//    Relais  D8  

#define SS_PIN          10         
#define RST_PIN         9         
#define REL_PIN         8         
#define DIR_PIN         5
#define STP_PIN        2
#define stepsPerRevolution 200

long chipID;
long chipIDlast;
long gross[4];
long klein[4];
int counter =0;
char Portion = 0;

// MFRC522-Instanz erstellen
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  //Relais auschalten
  pinMode(REL_PIN, OUTPUT);
  digitalWrite(REL_PIN, HIGH);
  
  //Relais-Ausgänge vorbereiten
  pinMode(DIR_PIN, OUTPUT); 
  pinMode(STP_PIN, OUTPUT);

  Serial.begin(115200);

  // initialisieren
  SPI.begin();
  mfrc522.PCD_Init();
  delay(10);
  Serial.println("");

  // Details vom MFRC522 RFID READER / WRITER ausgeben
  mfrc522.PCD_DumpVersionToSerial();  
  Serial.println("RFID-Chip auflegen, um UID anzuzeigen...");
}

void loop() {
  chipIDlast = 99999;
  // Sobald ein Chip 5s lang aufliegt, startet das popcorning...
  if (mfrc522.PICC_IsNewCardPresent()){
    chipID = 0;
    mfrc522.PICC_ReadCardSerial();
    for (byte i = 0; i < mfrc522.uid.size; i++){
      chipID=((chipID+mfrc522.uid.uidByte[i])*10);
    }
    counter = 1;
    chipIDlast = chipID;
    Serial.print("Counter:   ");
    Serial.print(counter);
    Serial.print("   chipIDlast:  ");
    Serial.print(chipIDlast);
    Serial.print("   chipID:   ");
    Serial.println(chipID);

  }  

   /*if (mfrc522.PICC_IsNewCardPresent()){*/
    chipID = 0;
    mfrc522.PICC_ReadCardSerial();
    for (byte i = 0; i < mfrc522.uid.size; i++){
      chipID=((chipID+mfrc522.uid.uidByte[i])*10);
    }
    if (chipIDlast == chipID) {
      counter = 2;
      Serial.print(" Counter:   ");
      Serial.print(counter);
      Serial.print("   chipIDlast:  ");
      Serial.print(chipIDlast);
      Serial.print("   chipID:   ");
      Serial.println(chipID);
    }
    if (chipIDlast != chipID) {
      Serial.println("Es fängt von vorn an!");
      return;
    }
        
  /*}*/  
    Serial.print("  Counter:   ");
    Serial.print(counter);
    Serial.print("   chipIDlast:  ");
    Serial.print(chipIDlast);
    Serial.print("   chipID:   ");
    Serial.println(chipID);
    int index = 0;
    step(true, DIR_PIN, STP_PIN, 600);
    digitalWrite(REL_PIN, LOW);
    delay(100000);
    digitalWrite(REL_PIN, HIGH);
    delay(300000); 
    //popcorngross();

    
    do {
      chipID = 0;
      chipID=chiplesen();
      index++;
      Serial.print("Index:  ");
      Serial.println(index);
      Serial.print("  Counter:   ");
      Serial.print(counter);
      Serial.print("   chipIDlast:  ");
      Serial.print(chipIDlast);
      Serial.print("   chipID:   ");
      Serial.println(chipID);
      Serial.println("  Ich tu nix!");
      delay(1000);
    } while (chipIDlast == chipID);
    
    Serial.println("Ich tu wieder was!");
    // Danach 5 Sekunden pausieren, um mehrfaches lesen /ausführen zu verhindern
    delay(5000);
 
 /*if (mfrc522.PICC_IsNewCardPresent() == true){
    Serial.print("Chip immer noch da?");

    //CardID resetten
    chipID = 0;
      
    mfrc522.PICC_ReadCardSerial();

    // Hier wird die ID des Chips in die Variable chipID geladen
    for (byte i = 0; i < mfrc522.uid.size; i++){
      chipID=((chipID+mfrc522.uid.uidByte[i])*10);
    }
    Serial.print("  chipID:  ");
    Serial.print(chipID);
    Serial.print("  chipIDfirst:  ");
    Serial.println(chipIDfirst);
    if (chipID == chipIDfirst)
      {
        Serial.print(" JA! ");
        digitalWrite(REL_PIN,LOW);
        while(stepper.currentPosition() != 400)
          {
          stepper.setSpeed(200);
          stepper.runSpeed();
          }
  delay(1000);
  // Reset the position to 0:
  stepper.setCurrentPosition(0);
        delay(1800);
      }
    chipID = 0;
    Serial.print("ChipID2: ");
    Serial.println(chipID);

  }

*/  
  
}

long chiplesen() {
 if (mfrc522.PICC_IsNewCardPresent()){
  long ID = 0;
  mfrc522.PICC_ReadCardSerial();
    for (byte i = 0; i < mfrc522.uid.size; i++){
      ID=((ID+mfrc522.uid.uidByte[i])*10);
    }
  Serial.println(ID);
  return ID;
 } 
}  

int popcorngross() {
    digitalWrite(REL_PIN, HIGH);
    delay(2000);
    digitalWrite(REL_PIN, LOW);
}

void step(boolean dir, byte dirPin, byte stepperPin, int steps)
{
  digitalWrite(dirPin, dir);
  delay(50);
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepperPin, HIGH);
    delayMicroseconds(400);  
    digitalWrite(stepperPin, LOW);
    delayMicroseconds(400);  
  }
}
