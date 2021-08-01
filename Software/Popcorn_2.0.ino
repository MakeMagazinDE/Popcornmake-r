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
#define STP_PIN         2
#define stepsPerRevolution 200
#define Anzahlgross     4 // Hier die Anzahl der verwendeten grossen Schüsseln eintragen
#define Anzahlklein     4 // Hier die Anzahl der verwendetetn kleinen Schüsseln eintragen

long chipID;
long chipIDlast;
int counter = 0;
int Portion = 0;
int Zeitklein = 120; // Hier die Zeit in Sekunden eintragen, die zum Zubereitung einer kleinen Portion gebraucht wird
int Zeitgross = 180; // Hier die Zeit in Sekunden eintragen, die zum Zubereiten einer großen Portion benötigt wird
int Abkuehlzeit = 300; //Hier die Pausenzeit zwischen zwei Portionen einstellen
long klein[Anzahlklein] = {1839980,2446980,2372280,1732780}; //Hier die IDs der kleinen Schüsseln jeweils eintragen
long gross[Anzahlgross] = {1589880,2400880,1578780,2343080}; // Hier die IDs der grossen Schüsseln jeweils eintragen

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
  // Ausgabe der registrierten Schüssel-IDs und die dazugehörenden Größen
  Serial.print(Anzahlklein);
  Serial.println(" kleine Schüsseln registiert");
  Serial.println("Nr. Größe ID");
  for (int i = 0; i < Anzahlklein; i++) {
    Serial.print(i);
    Serial.print("   klein ");
    Serial.println(klein[i]);
  } 
  Serial.print(Anzahlgross);
  Serial.println(" grosse Schüsseln registiert");
  Serial.println("Nr. Größe ID"); 
  for (int i = 0; i < Anzahlgross; i++) {
    Serial.print(i);  
    Serial.print("   gross ");
    Serial.println(gross[i]);
  }
  //Ende der ID-Ausgabe
  
  // initialisieren
  SPI.begin();
  mfrc522.PCD_Init();
  delay(10);
  Serial.println("");

  // Details vom MFRC522 RFID READER / WRITER ausgeben
  mfrc522.PCD_DumpVersionToSerial();  
  Serial.println("Schüssel aufstellen, um Popcorn-Produktion zu starten");
}

void loop() {
   
  chipIDlast = 0;
   // Sobald ein bekannter Chip erkannt wird, startet das popcorning...
    chipID = chiplesen();
    if (chipID == 0)
    {
      //Serial.println("Nix zu tun!");
      return;
    }
    Serial.print("Gelesene chipID: ");
    Serial.println(chipID);
    chipIDlast = chipID;
    delay(1000);
    Portion = 0;
    for (int i = 0; i <= (Anzahlklein-1); i++)
    {
       if (klein[i] == chipID)
       {
         Portion = 1;
         counter = counter + 1;
         Serial.print(chipID);
         Serial.println(" ist bekannt: kleine Portion");
       }
    }
    
    for (int i = 0; i <= Anzahlgross-1; i++)
    {
       if (gross[i] == chipID)
       {
         Portion = 2;
         counter = counter + 1;
         Serial.print(chipID);
         Serial.println(" ist bekannt: grosse Portion");
       }
    }
    Serial.print("Portionsgröße ist ");
    Serial.print(Portion);
    Serial.print(" Durchgang: ");
    Serial.println(counter);
           
    if (Portion == 1)
    {
      step(true, DIR_PIN, STP_PIN, 300);
      digitalWrite(REL_PIN, LOW);
      for (int i = 0; i <= Zeitklein; i++)
      {
        delay(1000);
        Serial.print("Noch ");
        Serial.print(Zeitklein -i);
        Serial.println(" Sekunden");
      }  
      digitalWrite(REL_PIN, HIGH);
      Portion = 0; 
    }
    if (Portion == 2)
    {
      step(true, DIR_PIN, STP_PIN, 600);
      digitalWrite(REL_PIN, LOW);
      for (int i = 0; i <= Zeitgross; i++)
      {
        delay(1000);
        Serial.print("Noch ");
        Serial.print(Zeitgross -i);
        Serial.println(" Sekunden");
      }      
      digitalWrite(REL_PIN, HIGH);
      Portion = 0; 
    }
    
    do {
      chipID=chiplesen();
      Serial.println("Schüssel wegnehmen!");
    } while (chipIDlast == chipID);
    for (int i = 0; i < Abkuehlzeit; i++)
    {
      delay(1000);
      Serial.print("Noch ");
      Serial.print(Abkuehlzeit -i);
      Serial.println(" Sekunden abkühlen");
    }
    Serial.println("Ich tu wieder was!");
    
}

long chiplesen()  {
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return 0;
  }
  long code=0;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    code=((code+mfrc522.uid.uidByte[i])*10);
  }
  return code;
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
