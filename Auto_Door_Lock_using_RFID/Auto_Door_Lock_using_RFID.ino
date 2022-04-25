#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <RFID.h> //library RFID
#include <SPI.h> //library serial parallel interface

#define sda 10 //Pin Serialdata (SDA)
#define rst 9 //pin Reset
#define PLC 6
#define buzzer 7
RFID rfid(sda,rst);

String tag_card = "3939135349";
String tag_biru = "7171170110234";

void setup() {
  lcd.begin();
  lcd.setCursor(0,0);
  lcd.print("RFID Reader");
  lcd.setCursor(0,1);
  lcd.print("Ready!");
  SPI.begin(); //Prosedur antarmuka SPI
  rfid.init(); //Memulai inialisasi module RFID
  delay(2000);
  lcd.clear();
//  Serial.begin(9600);
  pinMode(PLC, OUTPUT);
  pinMode(buzzer, OUTPUT);
  lcd.print("Place RFID Tag!");
}

void loop(){
  
  if(rfid.isCard()){
    if(rfid.readCardSerial()){
      String new_card = String(rfid.serNum[1]) + String(rfid.serNum[1]) + String(rfid.serNum[2]) + String(rfid.serNum[3]) + String(rfid.serNum[4]);
      lcd.clear(); 

      if(new_card==tag_card){
        lcd.setCursor(0,0);
        lcd.print("Welcome");
        lcd.setCursor(0,1);
        lcd.print("home!");
        digitalWrite(PLC, HIGH);
      }
//      else if(new_card==tag_biru){
//        lcd.setCursor(0,0);
//        lcd.print("Welcome");
//        lcd.setCursor(0,1);
//        lcd.print("User 2");
//        digitalWrite(PLC, HIGH);
//      }
      else{
        lcd.setCursor(0,0);
        lcd.print("Unknown ID");
        lcd.setCursor(0,1);
        lcd.print("Cannot Access!");
        digitalWrite(PLC, HIGH);
        digitalWrite(buzzer, HIGH);
        
      }
      delay(2000);
      digitalWrite(PLC, LOW);
      digitalWrite(buzzer, HIGH);                
      lcd.clear(); 
      lcd.print("Place RFID Tag!");
    }
  }   
}
