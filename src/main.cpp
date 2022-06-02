#include <Arduino.h>
#include <SPI.h>
#include "SSD1306Wire.h"
#include "time.h"
#include <MFRC522.h>
#include "FS.h"
#include "SD.h"
#include "WiFi.h"


#define SS_PIN_RFID 5
#define CS_PIN_SD 15
#define RST_PIN_RFID 17
// SCK RFID I SCK SD pin18
// MOSI RFID I MOSI SD pin23
// MISO RFID I MISO SD pin19 (amb resistència)

#define SDA_PIN 21
#define SCL_PIN 22

#define LED_READY 16
#define LED_NOT 4
#define LED_WAIT 0
#define LED_OK 2

#define BUZZER 25

// obtenir hora:
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

boolean rfid_read=false;

boolean waiting=false;

boolean pantalla=false;

boolean yes,no;

SSD1306Wire display(0x3c, SDA_PIN, SCL_PIN);
void readRFID(void );

byte nuidPICC[4] = {0, 0, 0, 0};
MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(SS_PIN_RFID, RST_PIN_RFID);

void init_wifi();

void readRFID(void );
void manage_leds(void );

void printHex(byte *buffer, byte bufferSize);
void printDec(byte *buffer, byte bufferSize);


String user;
String expiration;
String id;
String status="0";
String hora_data;

typedef struct
  {
    String name, expiration, id;
  } User;

User* users;

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.println(); 
}

void getDataHora()
{
    hora_data="";
    struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
    char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  char timeMinute[3];
  strftime(timeMinute,3, "%M", &timeinfo);
  char timeSecond[3];
  strftime(timeSecond,3, "%S", &timeinfo);
  char timeDay[3];
  strftime(timeDay,3, "%d", &timeinfo);
  char timeMonth[3];
  strftime(timeMonth,3, "%m", &timeinfo);
  char timeYear[5];
  strftime(timeYear,5, "%Y", &timeinfo);
  hora_data+=timeHour;
  hora_data+=+":";
  hora_data+=timeMinute;
  hora_data+=":";
  hora_data+=timeSecond;
  hora_data+=" ";
  hora_data+=timeDay;
  hora_data+="/";
  hora_data+=timeMonth;
  hora_data+="/";
  hora_data+=timeYear;
}

User getUser(String id)
{
  boolean found=false;
  int i=0;
  while(!found && i<sizeof(users))
  {
    if(users[i].id==id) return users[i];
    i++;
  }

  User error;
  error.expiration="-";
  error.name="Convidat";
  error.id=id;
  return error;
}

void read_rfid(void * parameter){
  for(;;){
    if(!rfid_read)
    {
        digitalWrite(LED_READY,HIGH);
        for (byte i = 0; i < 6; i++) {
          key.keyByte[i] = 0xFF;
        }
        // Look for new 1 cards
        if ( ! rfid.PICC_IsNewCardPresent())
          ;
        // Verify if the NUID has been readed
        else if (  !rfid.PICC_ReadCardSerial());
        // Store NUID into nuidPICC array
        else{
        for (byte i = 0; i < 4; i++) {
          nuidPICC[i] = rfid.uid.uidByte[i];
        }

          //rfid_read=true;

digitalWrite(LED_READY,LOW);
        rfid_read=true;
        waiting=true;

        //Serial.print(F("RFID In dec: "));
        printDec(rfid.uid.uidByte, rfid.uid.size);
                printHex(rfid.uid.uidByte, rfid.uid.size);
          User now = getUser(id);
          getDataHora();
          user=now.name;
          id=now.id;
          expiration=now.expiration;
          status="1";

          pantalla=true;
        Serial.println();
        // Halt PICC
        rfid.PICC_HaltA();
        // Stop encryption on PCD
        rfid.PCD_StopCrypto1();
        }
    } 
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }

}

void manage_leds(void * parameter){
  for(;;){
        if(waiting) {
          digitalWrite(LED_WAIT,HIGH);
          digitalWrite(LED_READY,LOW);
        }
        else if(yes)
        {
          digitalWrite(LED_OK,HIGH);
          digitalWrite(BUZZER,HIGH);
          digitalWrite(LED_WAIT,LOW);
          delay(600);
          digitalWrite(BUZZER,LOW);
          delay(1400);
          digitalWrite(LED_OK,LOW);
          yes=false;

        }
        else if(no){
          digitalWrite(LED_NOT,HIGH);
          digitalWrite(LED_WAIT,LOW);
          delay(2000);
          digitalWrite(LED_NOT,LOW);
          no=false;
        }
      
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }

}

void notAllowed()
{
  status="0";
  rfid_read=false;
  waiting=false;
  no=true;
}

void allowed()
{
  status="0";
  rfid_read=false;
  yes=true;
  waiting=false;
}

void sd_to_database(){
  Serial.printf("Llegint: %s\n", "/users.txt");

  File file = SD.open("/users.txt");
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }

  int i = 0;
  while(file.available()){
    //Serial.write(file.read());
    file.readStringUntil('\n');
    i++;
  }
  file.close();
  users = new User[i+1];
  file = SD.open("/users.txt");
  i=0;
  String id;
  while(file.available()){
    users[i].id=file.readStringUntil(',');
    users[i].name=file.readStringUntil(',');
    users[i].expiration=file.readStringUntil('\n');
    i++;
  }
}

void setup() {
  Serial.begin(115200);
  init_wifi();
  Serial.println("Inicialitzant sistema...");
  pinMode(LED_READY,OUTPUT);
  pinMode(LED_NOT,OUTPUT);
  pinMode(LED_WAIT,OUTPUT);
  pinMode(LED_OK,OUTPUT);
  pinMode(BUZZER,OUTPUT);
  rfid_read=true;
  SPI.begin();
  rfid.PCD_Init();
  Serial.print(F("Reader :"));
  rfid.PCD_DumpVersionToSerial();
  display.init();
  display.flipScreenVertically();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.clear();
  display.drawString(128/2, 24, "Carregant...");
  display.display();
  xTaskCreate(
    read_rfid, // Function that should be called
    "Read RFID", // Name of the task (for debugging)
    10000, // Stack size (bytes)
    NULL, // Parameter to pass
    1, // Task priority
    NULL // Task handle
  );

    xTaskCreate(
    manage_leds, // Function that should be called
    "Manage LEDs", // Name of the task (for debugging)
    1000, // Stack size (bytes)
    NULL, // Parameter to pass
    1, // Task priority
    NULL // Task handle
  );

  if(!SD.begin(15)){
    Serial.println("No s'ha pogut muntar la targeta SD");
      display.setFont(ArialMT_Plain_10);
      display.clear();
      display.drawString(128/2, 30, "E: No s'ha pogut muntar la SD");
      display.display();
    return;
  }
    uint8_t cardType = SD.cardType();

    
    if(cardType == CARD_NONE){
    Serial.println("No hi ha cap targeta SD connectada");
      display.setFont(ArialMT_Plain_10);
      display.clear();
      display.drawString(128/2, 30, "E: Cap SD connectada");
      display.display();
    return;
  }

  Serial.print("Tipus SD: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
   uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  Serial.println("Llegint base de dades...");
  sd_to_database();
  Serial.println("Base de dades llegida.");

   // Init and get the time
  while (WiFi.status() != WL_CONNECTED){delay(100);}
  delay(1500);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  Serial.println("Sincronitzant rellotge...");
  while(!getLocalTime(&timeinfo)) delay(1000);
  Serial.println("Rellotge sincronitzat");
  printLocalTime();

  display.clear();
  display.drawString(128/2, 24, WiFi.localIP().toString());
  display.display();


  rfid_read=false;
}

void loop()
{
if(pantalla)
{
  display.clear();
            display.setTextAlignment(TEXT_ALIGN_CENTER);
          display.setFont(ArialMT_Plain_16);
          display.drawString(128/2, 4, "Benvingut/da");
          display.setFont(ArialMT_Plain_10);
          display.drawString(128/2, 22, user);
              display.setFont(ArialMT_Plain_10);

          display.drawString(128/2, 40, "Esperi si us plau");
          display.display();
          pantalla=false;
}
else if(yes)
{
    display.clear();
            display.setTextAlignment(TEXT_ALIGN_CENTER);
          display.setFont(ArialMT_Plain_16);
          display.drawString(128/2, 16, "Accés");
          display.drawString(128/2, 32, "autoritzat");
          display.display();
          delay(2500);
          display.clear();
          display.drawString(128/2, 16, "Passi la");
          display.drawString(128/2, 32, "targeta");
          display.display();
}
else if(no)
{
    display.clear();
            display.setTextAlignment(TEXT_ALIGN_CENTER);
          display.setFont(ArialMT_Plain_16);
          display.drawString(128/2, 16, "Accés");
          display.drawString(128/2, 32, "denegat");
          display.display();
          delay(2500);
          display.clear();
          display.drawString(128/2, 16, "Passi la");
          display.drawString(128/2, 32, "targeta");
          display.display();
}
}

void printHex(byte *buffer, byte bufferSize) {
 id="";
 for (byte i = 0; i < bufferSize; i++) {
   Serial.print(buffer[i] < 0x10 ? " 0" : " ");
   Serial.print(buffer[i], HEX);
   id+=buffer[i];
 }
 Serial.println(id);
}

/**
   Helper routine to dump a byte array as dec values to Serial.
*/
void printDec(byte *buffer, byte bufferSize) {
  struct tm timeinfo;
      if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.print(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print(" ");
 for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
 }
 // char txt[disp_out.length()];
}
