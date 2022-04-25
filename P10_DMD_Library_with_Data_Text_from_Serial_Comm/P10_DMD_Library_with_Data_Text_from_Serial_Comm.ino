#include <SPI.h>       
#include <DMD.h>        
#include <TimerOne.h>  
#include "SystemFont5x7.h"
#include "Arial_Black_16_ISO_8859_1.h"
//#include "Droid_Sans_12.h"
#define DISPLAYS_ACROSS 2
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);
DMD kotak(DISPLAYS_ACROSS, DISPLAYS_DOWN);
///////////////////////////////////////////////////////////////////////////////////////////
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); // RX, TX
///////////////////////////////////////////////////////////////////////////////////////////
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
String jadwal, message, currency, weather, news, prev_message;
String showtext;
char code;
////////////////////////////////////////
unsigned long interval=40; // the time we need to wait
unsigned long previousMillis=0; // millis() returns an unsigned long.
////////////////////////////////////////
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);  
  mySerial.println("Ready");
  Timer1.initialize( 3000 );          
  Timer1.attachInterrupt( ScanDMD ); 
  kotak.clearScreen( true );   
  kotak.drawBox(  0,  0, (32*DISPLAYS_ACROSS)-1, (16*DISPLAYS_DOWN)-1, GRAPHICS_NORMAL );
  dmd.clearScreen( true );
  dmd.selectFont(Arial_Black_16_ISO_8859_1);
}
int i=0;
bool do_show = LOW;
bool first_cycle=LOW;
void loop() {
  if(do_show==HIGH){
     unsigned long currentMillis = millis(); // grab current time
      if ((unsigned long)(currentMillis - previousMillis) >= interval) {
        dmd.stepMarquee(-1,0);
        previousMillis = millis();
      }
    }
  while (Serial.available()) {
    do_show=LOW;
    char inChar = (char)Serial.read();
//    // add it to the inputString:
    inputString += inChar;
    if (inChar == '\n') {
      dmd.clearScreen( true );
      mySerial.print(string2char(inputString));       
      dmd.drawMarquee(string2char(inputString),strlen(string2char(inputString)),(32*DISPLAYS_ACROSS)-1, 0);
      do_show=HIGH;   
      inputString="";
    }
  }
}

//void serialEvent() {
//  while (Serial.available()) {
//    do_show=LOW;
//    dmd.clearScreen( true );
//    char inChar = (char)Serial.read();
////    // add it to the inputString:
//    if(first_cycle==LOW){
//      code = inChar;
//      first_cycle=HIGH;
//    }
//    inputString += inChar;
//    if (inChar == '\n') {
//      mySerial.print(inputString);
//      first_cycle=LOW;
//      stringComplete = true;
//      switch (code) {
//        case 'P':
//          jadwal = inputString;
//          break;
//        case 'M':
//          message = inputString;
//          break;
//        case 'C':
//          currency = inputString;
//          break;
//        case 'W':
//          weather = inputString;
//          break;
//        case 'N':
//          news = inputString;
//          break;
//        case '1':
//          showtext=message;
//          dmd.drawMarquee(string2char(message),strlen(string2char(message)),(30*DISPLAYS_ACROSS)-1,0);
//          do_show=HIGH;
//          i=9;
//          break;
//        case '2':
//          showtext=weather;
//          dmd.drawMarquee(string2char(weather),strlen(string2char(weather)),(30*DISPLAYS_ACROSS)-1,0);
//          do_show=HIGH;
//          break;
//        case '3':
//          showtext=currency;
//          dmd.drawMarquee(string2char(currency),strlen(string2char(currency)),(30*DISPLAYS_ACROSS)-1,0);
//          do_show=HIGH;
//          break;
//        case '4':
//          showtext=jadwal;
//          dmd.drawMarquee(string2char(jadwal),strlen(string2char(jadwal)),(30*DISPLAYS_ACROSS)-1,0);
//          do_show=HIGH;
//          break;
//        case '5':
//          showtext=news;
//          dmd.drawMarquee(string2char(news),strlen(string2char(news)),(30*DISPLAYS_ACROSS)-1,0);
//          do_show=HIGH;
//          break;
//      }
//      inputString="";
//    }
//  }
//}

void ScanDMD()
{ 
  kotak.scanDisplayBySPI();
  dmd.scanDisplayBySPI();
}
char *string2char(String command){
  if(command.length()!=0){
    char *p = const_cast<char*>(command.c_str());
    return p;
  }
}
