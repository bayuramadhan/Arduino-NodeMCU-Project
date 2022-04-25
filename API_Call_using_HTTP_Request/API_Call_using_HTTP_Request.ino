extern "C" {
  #include "user_interface.h"
}
 //library esp8266 default
//////////////////instalasi timer 5 menit/////////////////////
os_timer_t timer;
boolean tickOccured = false;

void timer_callback(void *pArg) {
  tickOccured = true;
}
//////////////////////////////////////////////////////////////

////////memasukkan library wifi, wificlient dan thingspeak////
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include "ThingSpeak.h"
#include "secrets.h"
//////////////////////////////////////////////////////////////

/////////////Setting Access Point dan PWD/////////////////////
const char* ssid     = "Polman 407";
const char* password = "PolmaN@407";
WiFiClient client;
//////////////////////////////////////////////////////////////

//////Settingan ID channel Thingspeak, ada di secret.h////////
unsigned long counterChannelNumber = SECRET_CH_ID_COUNTER;
const char * myCounterReadAPIKey = SECRET_READ_APIKEY_COUNTER;
unsigned int counterFieldNumber = 1; 
//////////////////////////////////////////////////////////////

void setup() { 
/////////////Inisialisasi memulai koneksi Wifi////////////////
  Serial.begin(9600);    
  WiFi.disconnect(); // Disconnect AP
  WiFi.mode(WIFI_STA);  
  WiFi.begin(ssid, password); // Connect to WIFI network
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  Serial.println(".");
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(ssid);
//  Serial.print("IP address: ");
//  Serial.println(WiFi.localIP());
//////////////////////////////////////////////////////////////
    delay(5000);
//    update_salat();
//    update_ThingSpeak();
//    update_currency();
//    update_news();
//    update_weather();
    ThingSpeak.begin(client);  // Initialize ThingSpeak
    delay(1000);    
    os_timer_setfn(&timer, timer_callback, NULL); //inisialisasi timer
    os_timer_arm(&timer, 300000, true); //inisialisasi durasi timer (300000 milisecond = 300 detik = 5 menit)
}

void loop() {
  read_thingspeak();
  delay(1000); 
}

//////////////////////membaca nilai dari channel Thingspeak untuk didisplay//////////////////////
int count, prev_count;
void read_thingspeak(){
  int statusCode = 0;
  count = ThingSpeak.readLongField(counterChannelNumber, counterFieldNumber, myCounterReadAPIKey);  
   // Check the status of the read operation to see if it was successful
  statusCode = ThingSpeak.getLastReadStatus();
  if(statusCode == 200){
    if(count!=prev_count){
      if(count==1){
        update_ThingSpeak(); //jika nilai pembacaan thingspeak 1 maka menampilkan message
      }
      if(count==2){
        update_weather();    //jika nilai pembacaan thingspeak 2 maka menampilkan weather
      }
      if(count==3){
        update_currency();   //jika nilai pembacaan thingspeak 3 maka menampilkan currency
      }
      if(count==4){
        update_salat();      //jika nilai pembacaan thingspeak 4 maka menampilkan jadwal salat
      }
      if(count==5){
        update_news();       //jika nilai pembacaan thingspeak 5 maka menampilkan news   
      }
//      Serial.println(String(count));
      prev_count=count; 
    }
    else{
      if(tickOccured) {     //jika tidak ada perubahan nilai pembacaan channel maka update setiap 5 menit
        if(count==1){
          update_ThingSpeak();
        }
        if(count==2){
          update_weather();
        }
        if(count==3){
          update_currency();
        }
        if(count==4){
          update_salat();
        }
        if(count==5){
          update_news();      
        }        
        tickOccured = false;
      }
      yield();
    }

  }
  else{
    Serial.println("Problem reading channel. HTTP error code " + String(statusCode)); 
    prev_count=0;          
  }  
}

//////////////////////mendapatkan info jadwal salat//////////////////////
#define SALAT_API_KEY "bd099c5825cbedb9aa934e255a81a5fc"
#define SALAT_URL "muslimsalat.com"
void update_salat(){
  const char request[] = 
    "GET /jakarta.json HTTP/1.1\r\n"
//    "User-Agent: ESP8266/0.1\r\n"
//    "Accept: */*\r\n"
    "Host: " SALAT_URL "\r\n"
    "Connection: close\r\n"
    "\r\n";
  if(client.connect(SALAT_URL, 80)){
    client.print(request);
//    Serial.print(request);
    client.flush();
  }

  delay(1000);
  String text;
  while (client.available()) {
    char c = client.read();
//    Serial.write(c);
    text = text + c;
  }
  // if the server's disconnected, stop the client
  if (!client.connected()) {
//    Serial.println();
//    Serial.println("Disconnecting from server...");
    client.stop();
    int cutNo = text.indexOf('{');
    int cutNo2 = text.lastIndexOf('}');
    String output = text.substring(cutNo+1, cutNo2);
//    Serial.print("TRIM = ");Serial.println(output);
    int temp_bb = output.indexOf('{');
    int temp_ba = output.indexOf('}');
    int solat_bb = output.indexOf('{', temp_bb+1);
    int solat_ba = output.indexOf('}', temp_ba+1);
    String solat = output.substring(solat_bb+1, solat_ba);
    solat.replace('"', ' '); 
    Serial.println("Pray Time: " + solat);
  }
}    

//////////////////////mendapatkan pesan singkat//////////////////////
#define thingSpeakAddress "api.thingspeak.com"
void update_ThingSpeak(){
  const char request[] = 
    "GET /channels/777586/status.json?results=1 HTTP/1.1\r\n"
//    "User-Agent: ESP8266/0.1\r\n"
    "Accept: */*\r\n"
    "Host: " thingSpeakAddress "\r\n"
    "Connection: close\r\n"
    "\r\n";
//  Serial.println(request);
  client.connect(thingSpeakAddress, 80);
  client.print(request);
  client.flush();
  delay(1000);
  String text;
  while (client.available()) {
    char c = client.read();
//    Serial.write(c);
    text = text + c;
  }
  // if the server's disconnected, stop the client
  if (!client.connected()) {
//    Serial.println();
//    Serial.println("Disconnecting from server...");
    client.stop();
    int cutNo = text.indexOf('\\');
    int cutNo2 = text.lastIndexOf('\\');
    String output = text.substring(cutNo+2, cutNo2);
    Serial.println("Message: " + output);
  }
}

//////////////////////mendapatkan info currency//////////////////////
#define currency_server "api.ratesapi.io"
void update_currency(){
  const char request[] = 
    "GET /api/latest?base=USD HTTP/1.1\r\n"
//    "User-Agent: ESP8266/0.1\r\n"
//    "Accept: */*\r\n"
    "Host: " currency_server "\r\n"
    "Connection: close\r\n"
    "\r\n";
//  Serial.println(request);
  client.connect(currency_server, 80);
  client.print(request);
  client.flush();
  delay(1000);
  String text;
  while (client.available()) {
    char c = client.read();
//    Serial.write(c);
    text = text + c;
  }
  // if the server's disconnected, stop the client
  if (!client.connected()) {
//    Serial.println();
//    Serial.println("Disconnecting from server...");
    client.stop();
    int cutNo = text.indexOf('{');
    int cutNo2 = text.lastIndexOf('}');
    String output = text.substring(cutNo+1, cutNo2-1);
//    Serial.println(output);
    String v_currency[] = {"date", "IDR", "EUR", "CNY", "JPY", "GBP", "SGD", "AUD"};
    String message = "Currency: " + output.substring(output.indexOf("date"));
    String base = ", base :USD, ";
    message.replace('"', ' '); 
    message+= base;
//    Serial.println(message);
    for(int i=1;i<=7;i++){
      int a = output.indexOf(v_currency[i]);
      int b = output.indexOf('"', a+5);
      v_currency[i] = output.substring(a, b-8);
      v_currency[i].replace('"', ' '); 
      v_currency[i]+= " ";
      message+=v_currency[i];
    }
    int a = message.indexOf('}');
    message.remove(a, 1);
    int b = message.lastIndexOf(',');
    message.remove(b, 1);
//    Serial.println(output);
    Serial.println(message);
  }
}

//////////////////////mendapatkan info weather//////////////////////
#define weather_server "api.openweathermap.org"
void update_weather(){
  const char request[] = 
    "GET /data/2.5/weather?q=Jakarta,ID&APPID=be14d3624a89fce56e0d458acbb955e7 HTTP/1.1\r\n"
//    "User-Agent: ESP8266/0.1\r\n"
    "Accept: */*\r\n"
    "Host: " weather_server "\r\n"
    "Connection: close\r\n"
    "\r\n";
//  Serial.println(request);
  client.connect(weather_server, 80);
  client.print(request);
  client.flush();
  delay(1000);
  String text;
  while (client.available()) {
    char c = client.read();
//    Serial.write(c);
    text = text + c;
  }
  // if the server's disconnected, stop the client
  if (!client.connected()) {
//    Serial.println();
//    Serial.println("Disconnecting from server...");
    client.stop();
    int cutNo = text.indexOf('{');
    int cutNo2 = text.lastIndexOf('}');
    for(int i=0;i<4;i++)
    {
    cutNo2 = text.lastIndexOf('}', cutNo2-2);      
    }   
    String output = text.substring(cutNo+1, cutNo2+1);
//    Serial.println(output);
    String v_weather[] = {"main", "temp", "pressure", "humidity", "temp_min", "temp_max"};
    String message = "Weather: Jakarta : ";
    for(int i=0;i<=4;i++){
      int a = output.indexOf(v_weather[i]);
      int b = output.indexOf('"', a+4+v_weather[i].length());
      v_weather[i] = output.substring(a, b+1);
      v_weather[i].replace('"', ' '); 
//      Serial.println(v_weather[i]);
//      message+=v_weather[i];
    } 
    v_weather[0]+=", ";
    int c = output.indexOf(v_weather[5]);
    int d = output.lastIndexOf('}');      
    v_weather[5] = output.substring(c, d);
    v_weather[5].replace('"', ' '); 
//    convert temperature
    int x = v_weather[1].indexOf(':');
    int y = v_weather[1].indexOf(',');
    String xy = v_weather[1].substring(x+1, y);
    int t_c = xy.toInt() - 273;
    xy = v_weather[1].substring(0, x+1);
    v_weather[1] = xy + t_c + "°c, ";
//    pressure
    v_weather[2].remove(v_weather[2].indexOf(','));
    v_weather[2]+="hpa, ";
//    humidity
    v_weather[3].remove(v_weather[3].indexOf(','));
    v_weather[3]+="%, ";
//    convert temperature min
    x = v_weather[4].indexOf(':');
    y = v_weather[4].indexOf(',');
    xy = v_weather[4].substring(x+1, y);
    t_c = xy.toInt() - 273;
    xy = v_weather[4].substring(0, x+1);
    v_weather[4] = xy + t_c + "°c, ";
//    convert temperature max
    x = v_weather[5].indexOf(':');
    y = v_weather[5].indexOf(',');
    xy = v_weather[5].substring(x+1, y);
    t_c = xy.toInt() - 273;
    xy = v_weather[5].substring(0, x+1);
    v_weather[5] = xy + t_c + "°c";
    
    for(int i=0;i<=5;i++){
      message+=v_weather[i];
    }
    Serial.println(message);
  }
}

//////////////////////mendapatkan info weather//////////////////////
#define NEWS_API_KEY "425fce3e84874387bcef8df0a5968beb"
#define NEWS_URL "newsapi.org"
int get_news;
void update_news(){
  const char request[] = 
    "GET /v2/top-headlines?country=id&apiKey=" NEWS_API_KEY " HTTP/1.1\r\n"
//    "User-Agent: ESP8266/0.1\r\n"
//    "Accept: */*\r\n"
    "Host: " NEWS_URL "\r\n"
    "Connection: close\r\n"
    "\r\n";
//  Serial.println(request);
  client.connect(NEWS_URL, 80);
  client.print(request);
  client.flush();
  delay(1000);
  String text;
  while (client.available()) {
    char c = client.read();
//    Serial.write(c);
    text = text + c;
    if(c=='}'){
      get_news++;
      if(get_news==2){
        get_news=0;
        goto disconect;
      }
    }
  }
  // if the server's disconnected, stop the client
  if (!client.connected()) {
disconect:
//    Serial.println();
//    Serial.println("Disconnecting from server...");
    client.stop();
    int cutNo = text.indexOf('{');
    int cutNo2 = text.lastIndexOf('}');
    String output = text.substring(cutNo+1, cutNo2);
    String v_news[] = {"title", "description"};
    String message = "News: ";
    int a = output.indexOf(v_news[0]) + 8;
    int b = output.indexOf('"', a);
    v_news[0] = output.substring(a, b);
    int c;
    int d = output.indexOf(v_news[1]);
    int e = d;
    for(int i=0;i<4;i++){
      c = output.indexOf('"', d+1);
      d = c;
    }
    v_news[1] = output.substring(e, c-1);
    v_news[1].replace('"', ' ');
    message = message + v_news[0];// + " , " + v_news[1];
//    Serial.println(output);
    Serial.println(message);
  }  
}    
