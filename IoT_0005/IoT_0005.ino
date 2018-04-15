#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ArduinoJson.h>
#include <Fonts/FreeSerifBold18pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include "FS.h"
#include "Wire.h"
#include "u.h"
//#include "Timer.h"
bool f = 1;
long long time_z;
//Timer t;
int num = 0;
#include "global.h"
const char* ssid     = "Hack_Univ";                      // SSID
const char* password = "hackuniversity";                    // пароль
String weatherKey = "d8c7d8a6d5146b95d7e31215602740cd";  
String weatherLang = "&lang=ru";
String cityID = "498817"; 
// =======================================================================

const unsigned char apple [] = {
0x00, 0x00, 0x00, 0x01, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x07, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x0F,
0x80, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x00,
0x00, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x00,
0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xF0, 0x0F, 0xF8, 0x00, 0x00, 0x1F,
0xFE, 0x3F, 0xFE, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0,
0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x03, 0xFF, 0xFF, 0xFF,
0xFF, 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0x07, 0xFF, 0xFF, 0xFF, 0xFC, 0x00, 0x07, 0xFF,
0xFF, 0xFF, 0xFC, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFC, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xF8, 0x00,
0x0F, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x0F, 0xFF, 0xFF, 0xFF,
0xF8, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFC, 0x00, 0x0F, 0xFF,
0xFF, 0xFF, 0xFC, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFC, 0x00, 0x07, 0xFF, 0xFF, 0xFF, 0xFE, 0x00,
0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x03, 0xFF, 0xFF, 0xFF,
0xFF, 0xC0, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x01, 0xFF,
0xFF, 0xFF, 0xFF, 0xE0, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0,
0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x3F, 0xFF, 0xFF,
0xFF, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFC, 0x00, 0x00, 0x0F,
0xFF, 0xFF, 0xFC, 0x00, 0x00, 0x07, 0xFC, 0x0F, 0xF0, 0x00, 0x00, 0x01, 0xE0, 0x03, 0xE0, 0x00
};

WiFiClient client;

// Include the HTML, STYLE and Script "Pages"

#include "Page_Admin.h"
#include "Page_Market.h"
#include "Page_Script.js.h"
#include "Page_Style.css.h"
#include "Page_NetworkConfiguration.h"

bool connectOk = 0;

extern "C" {
#include "user_interface.h"
}

Ticker ticker;


os_timer_t myTimer;


//*** Normal code definition here ...

#define LED_PIN 2
#define buttonPin 12
int flag1;
//ADC_MODE (ADC_VCC); //Режим АЦП - замеряем вольтаж питания
float voltage1;
String result1;
String dataset;
long long timez;

long eventTime;


String chipID;

String SendHttp(String y) //функция отправки http get запроса, принимает url
{
  HTTPClient http;
  http.begin(y);
  String response = "error";
  short int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK)
  {
    Serial.print("HTTP response code ");
    Serial.println(httpCode);
    response = http.getString();
    Serial.println(response);
  }
  else
  {
    Serial.println("Error in HTTP request");
  }
  http.end();
  return response;
}

const char *weatherHost = "api.openweathermap.org";

void ResetAll(){ //чистка eeprom
  EEPROM.begin(512);
  // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++){
  EEPROM.write(i, 0);
  }
  EEPROM.end();
  ESP.reset();
}

void deepSleep(){ //режим глубокого сна(пока не используется)
     ESP.deepSleep(0, WAKE_RF_DEFAULT); 
     ESP.reset();
}


void update(){ //http прошивка микроконтроллера
  
  server.send ( 200, "text/plain", "Ok" );
  Serial.println("http://206.189.39.118/media/" + server.arg(0) + ".bin");
  t_httpUpdate_return ret = ESPhttpUpdate.update("http://206.189.39.118/media/" + server.arg(0) + ".bin");

        switch(ret) {
            case HTTP_UPDATE_FAILED:
                Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                break;

            case HTTP_UPDATE_NO_UPDATES:
                Serial.println("HTTP_UPDATE_NO_UPDATES");
                break;

            case HTTP_UPDATE_OK:
                Serial.println("HTTP_UPDATE_OK");
                break;
        }
}

String ipToString(IPAddress ip) { //преобразует ip в строку
  String s = "";
  for (int i = 0; i < 4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
}

bool CFG_saved = false;
void setup() {
  Wire.begin();
  pinMode(12,INPUT);
  SPIFFS.begin(); 
  //timer = micros();
  int WIFI_connected = false;
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(40,0);
  display.println("IOS 13");
  ESP.wdtDisable();
  display.drawBitmap(40, 12, apple, 48, 48, WHITE);
  display.display();
  delay(5000);
  display.clearDisplay();
  display.display();
  ESP.wdtEnable(1000);
  delay(500);
  pinMode(12,INPUT);
  //**** Загрузка конфигурации сети 
  EEPROM.begin(512);
  CFG_saved = ReadConfig();

  if (CFG_saved)  //если есть конфигурации сети
  {    
      // Подключение к точке досутпа
      Serial.println("Booting");
      
      WiFi.mode(WIFI_STA);
        flag1=1;
      //WiFi.begin(config.ssid.c_str(), config.password.c_str());
      WiFi.begin(config.ssid.c_str(), config.password.c_str());
      printConfig();
      WIFI_connected = WiFi.waitForConnectResult();
      if(WIFI_connected!= WL_CONNECTED ){
        Serial.println("Connection Failed! activating to AP mode...");
        Serial.println(ipToString(WiFi.localIP()));
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(40,0);
        display.println("Fail!!!");
        display.display();
      }
      else 
      {
        Serial.println(ipToString(WiFi.localIP()));
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(32,0);
        display.println(ipToString(WiFi.localIP()));
        display.display();
        connectOk = 1;
      }
  }

  if ( (WIFI_connected!= WL_CONNECTED) or !CFG_saved){
    // Конфигурации по умолчанию
    Serial.println("Setting AP mode default parameters");
    config.ssid = "IoT2";       
    config.password = "" ; 
    WiFi.mode(WIFI_AP);  
    WiFi.softAP(config.ssid.c_str());
    Serial.print("Wifi ip:");Serial.println(WiFi.softAPIP());
    flag1=0;
   }
   

    // ***Запуск http сервера
    server.on ( "/", []() {
      Serial.println("admin.html");
      server.send_P ( 200, "text/html", PAGE_AdminMainPage); 
    }  );
  
    server.on ( "/favicon.ico",   []() {
      Serial.println("favicon.ico");
      server.send( 200, "text/html", "" );
    }  );
  
    server.on ( "/config.html", send_network_configuration_html );
    server.on ( "/info.html", []() {
      Serial.println("info.html");
    }  );
    server.on ( "/update", update);
  
    //server.on ( "/appl.html", send_application_configuration_html  );
    //  server.on ( "/example.html", []() { server.send_P ( 200, "text/html", PAGE_EXAMPLE );  } );
    server.on ( "/style.css", []() {
      Serial.println("style.css");
      server.send_P ( 200, "text/plain", PAGE_Style_css );
    } );
    server.on ( "/microajax.js", []() {
      Serial.println("microajax.js");
      server.send_P ( 200, "text/plain", PAGE_microajax_js );
    } );
    server.on ( "/market", []() {
      Serial.println("market");
      server.send_P ( 200, "text/html", PAGE_Market );
    } );
    server.on ( "/admin/values", send_network_configuration_values_html );
    server.on ( "/admin/connectionstate", send_connection_state_values_html );
    //server.on ( "/admin/applvalues", send_application_configuration_values_html );
  
    server.onNotFound ( []() {
      Serial.println("Page Not Found");
      server.send ( 400, "text/html", "Page not Found" );
    }  );
    server.begin();
    Serial.println( "HTTP server started" );

    
timez = millis();
time_z = millis();
}



void show_oneme() {
  if ((millis()-time_z)<=60)
  {
	display.clearDisplay();
 	display.drawBitmap(0, 0,frame1 , 128, 64, WHITE);  // выводим изображение (X, Y, bmp, ширина, высота, цвет)
 	display.display();
  }
  else if ((millis()-time_z)<=120)
  {
 	display.clearDisplay();
  display.drawBitmap(0, 0,frame2 , 128, 64, WHITE);  // выводим изображение (X, Y, bmp, ширина, высота, цвет)
  display.display();
  }
  else if ((millis()-time_z)<=180)
  {
  display.clearDisplay();
  display.drawBitmap(0, 0,frame3 , 128, 64, WHITE);  // выводим изображение (X, Y, bmp, ширина, высота, цвет)
  display.display();
  }
  else if ((millis()-time_z)<=240)
  {
   display.clearDisplay();
  display.drawBitmap(0, 0,frame4 , 128, 64, WHITE);  // выводим изображение (X, Y, bmp, ширина, высота, цвет)
  display.display();
  }
  else if ((millis()-time_z)<=300)
  {
   display.clearDisplay();
  display.drawBitmap(0, 0,frame5 , 128, 64, WHITE);  // выводим изображение (X, Y, bmp, ширина, высота, цвет)
  display.display();
  }
  else if ((millis()-time_z)<=360)
  {
   display.clearDisplay();
  display.drawBitmap(0, 0,frame6 , 128, 64, WHITE);  // выводим изображение (X, Y, bmp, ширина, высота, цвет)
  display.display();
  }
  else if ((millis()-time_z)<=420)
  {
  display.clearDisplay();
  display.drawBitmap(0, 0,frame7 , 128, 64, WHITE);  // выводим изображение (X, Y, bmp, ширина, высота, цвет)
  display.display();
  }
  else if ((millis()-time_z)<=480)
  {
  display.clearDisplay();
  display.drawBitmap(0, 0,frame8 , 128, 64, WHITE);  // выводим изображение (X, Y, bmp, ширина, высота, цвет)
  display.display();
  }
  else if ((millis()-time_z)<=540)
  {
   display.clearDisplay();
  display.drawBitmap(0, 0,frame9 , 128, 64, WHITE);  // выводим изображение (X, Y, bmp, ширина, высота, цвет)
  display.display();
  }
  else if ((millis()-time_z)<=600)
  {
   display.clearDisplay();
  display.drawBitmap(0, 0,frame10 , 128, 64, WHITE);  // выводим изображение (X, Y, bmp, ширина, высота, цвет)
  display.display();
  }
  else if ((millis()-time_z)<=660)
  {
   display.clearDisplay();
  display.drawBitmap(0, 0,frame11 , 128, 64, WHITE);  // выводим изображение (X, Y, bmp, ширина, высота, цвет)
  display.display();
  }
  else if ((millis()-time_z)<=740)
  {
  display.clearDisplay();
  display.drawBitmap(0, 0,frame12 , 128, 64, WHITE);  // выводим изображение (X, Y, bmp, ширина, высота, цвет)
  display.display();
  }
  else if ((millis()-time_z)<=800)
  {
  display.clearDisplay();
  display.drawBitmap(0, 0,frame13 , 128, 64, WHITE);  // выводим изображение (X, Y, bmp, ширина, высота, цвет)
  display.display();
  }
  else if ((millis()-time_z)<=860)
  {
   display.clearDisplay();
  display.drawBitmap(0, 0,frame14 , 128, 64, WHITE);  // выводим изображение (X, Y, bmp, ширина, высота, цвет)
  display.display();
  }
  else if ((millis()-time_z)<=940)
  {
   display.clearDisplay();
  display.drawBitmap(0, 0,frame15 , 128, 64, WHITE);  // выводим изображение (X, Y, bmp, ширина, высота, цвет)
  display.display();
  }
  else if ((millis()-time_z)<=1000)
  {
   display.clearDisplay();
  display.drawBitmap(0, 0,frame16, 128, 64, WHITE);  // выводим изображение (X, Y, bmp, ширина, высота, цвет)
  display.display();
  }
  else if ((millis()-time_z)<=1060)
  {
  display.clearDisplay();
  display.drawBitmap(0, 0,frame17 , 128, 64, WHITE);  // выводим изображение (X, Y, bmp, ширина, высота, цвет)
  display.display();

  }
  else {time_z=millis();}
}

void loop() {
  
  // всегда готовы к прошивке
  server.handleClient();
  ESP.wdtDisable();
  show_oneme();
  ESP.wdtEnable(1000);
   // обновление watchdog таймера
   customWatchdog = millis();

}

