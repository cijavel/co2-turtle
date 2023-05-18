#include <Arduino.h>

// _______________
// EPD ePaper eINK
// ---------------
// BUSY 	  -> -1			Violett,
// RST 	  	-> -1 		RX2	Blau,
// DC 		  -> 27  		TX2	grün,
// CS 		  -> SS(5)		gelb,
// CLK 		  -> SCK(18)		orange,
// DIN /SDI -> MOSI(23) 	weiß,
// GND 		  -> GND,
// 3.3V 	  -> 3.3V
//#define GxEPD2_DRIVER_CLASS GxEPD2_213_Z98c // GDEY0213Z98 122x250, SSD1680


// _______________
// LED
// ---------------
// DATA -> 4

// _______________
// MH-Z19B
// ---------------
// GND -> GND
// VCC -> 5V
// RX  -> 17
// TX  -> 16

// _______________
// Bosch BME680
// ---------------
// GND -> GND
// VCC -> 5V
// SCL -> 22
// SDA -> 21
// SD0 -> -
// CS  -> -

//OOP
#include "BME680Handler.h"
#include "MHZ19Handler.h"
#include "Configuration.h"
#include "WiFiHandler.h"
//Sensor
#include "bsec.h"

//Wifi
#include <WiFi.h>

//Time
#include <time.h>

//EPD
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>

#include "../Font/BabelSans8pt7b.h"
#include "../Font/BabelSans9pt7b.h"
#include "../Font/BabelSans10pt7b.h"
#include "../Font/Inter_Regular12pt7b.h"
#include "../Font/Inter_Regular11pt7b.h"
#include "../Font/Inter_Regular10pt7b.h"
#include "../Font/Inter_Regular8pt7b.h"
#include "../Font/Inter_Regular7pt7b.h"

#include "../Font/Inter_Bold12pt7b.h"
#include "../Font/Inter_Bold10pt7b.h"


//OWN FILES
#include "symbol.h" // own symbol
// please rename credentials_example.h to credentials.h
#include <credentials.h>


// ---------------------------------------------
// Configuration
// ---------------------------------------------

// timezone
const String timezone = "CET-1CEST,M3.5.0,M10.5.0/3";

// EPD
// in separate file
#include "GxEPD2_display_selection_new_style.h"


// ---------------------------------------------
// Declaration
// ---------------------------------------------


// Timer
const long interval_EPD    = 60000*3;
const long interval_RAM    = 60000*0.5;

unsigned long prevtimer_EPD    = 0;
unsigned long prevtimer_RAM    = 0;

unsigned long currtimer_EPD    = 0;
unsigned long currtimer_RAM    = 0;

// --------------------------------------------------------------------------
// time functions
// --------------------------------------------------------------------------
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;


String localTime(String format)
{

  struct tm timeinfo;

  String time = "";
  char toutp[60];
  // Serial.printf("  Set Timezone to %s\n",timezone.c_str());
  setenv("TZ",timezone.c_str(),1);  //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
  tzset();

  if(!getLocalTime(&timeinfo))
  {
    time = "TIME: Failed to obtain";
  }
  else{
    strftime(toutp, sizeof(toutp), format.c_str(), &timeinfo);
    time = String(toutp);
  }
  return time;
}

// --------------------------------------------------------------------------
// EPD ePaper eINK
// --------------------------------------------------------------------------
String after_comma(String data){
  int a = data.toDouble();
  int b = (data.toDouble() - a) * 100;
  String sb = String(b);
  if (b < 10) {
    sb += "0";
  }
  return sb;
}

String before_comma(String data){
  int a = data.toDouble();
  return String(a);
}

void epd_w(String epd_name, String epd_date, String epd_time, String data_temp, String data_humidity, String data_airq, String data_co2  )
{
    display.init(9600);

    uint16_t color_temp = GxEPD_BLACK;
    uint16_t color_hum = GxEPD_BLACK;
    uint16_t color_aiq = GxEPD_BLACK;
    uint16_t color_co2 = GxEPD_BLACK;

    // Lines
    int line1 = 30;
    int line2 = 90;

    //Abstand
    int ab_temp = 20;
    int ab_hum  = 52;
    int ab_iaq  = 86;
    int ab_co2  = 115;

    //Limit
    if ((data_temp.toDouble())    < 26)   { color_temp = GxEPD_BLACK;} else { color_temp = GxEPD_RED;}
    if ((data_humidity.toDouble())< 70)   { color_hum  = GxEPD_BLACK;} else { color_hum  = GxEPD_RED;}
    if ((data_airq.toDouble())    < 300)  { color_aiq  = GxEPD_BLACK;} else { color_aiq  = GxEPD_RED;}
    if ((data_co2.toDouble())     < 1500) { color_co2  = GxEPD_BLACK;} else { color_co2  = GxEPD_RED;}


    display.fillScreen(GxEPD_WHITE); // set the background to white (fill the buffer with value for white)
    display.setFullWindow();
    display.setCursor(0, 0);

    display.setRotation(1); //0-3
    display.setFont(&Inter_Bold12pt7b);

    display.setTextColor(color_temp);
    display.setCursor(line1, ab_temp);
    display.print(before_comma(data_temp));
    display.setFont(&Inter_Bold10pt7b);
    display.print(".");
    display.print(before_comma(data_temp));


    display.setFont(&Inter_Bold12pt7b);
    display.setTextColor(color_hum);
    display.setCursor(line1, ab_hum );
    display.print(before_comma(data_humidity));
    display.setFont(&Inter_Bold10pt7b);
    display.print(".");
    display.print(before_comma(data_humidity));


    display.setFont(&Inter_Bold12pt7b);
    display.setTextColor(color_aiq);
    display.setCursor(line1, ab_iaq );
    display.print(before_comma(data_airq));
    display.setFont(&Inter_Bold10pt7b);
    display.print(".");
    display.print(before_comma(data_airq));


    display.setFont(&Inter_Bold12pt7b);
    display.setTextColor(color_co2);
    display.setCursor(line1, ab_co2 + 5);
    display.print(data_co2);


    display.setFont(&Inter_Regular10pt7b);


    display.setTextColor(color_hum);
    display.setCursor(line2, ab_hum);
    display.print(" %");

    display.setTextColor(color_aiq);
    display.setCursor(line2, ab_iaq);
    display.print(" IAQ");

    display.setTextColor(color_co2);
    display.setCursor(line2, ab_co2+2 );
    display.print(" ppm");

    display.drawInvertedBitmap(line2+5, ab_temp-20, bitmap_grad, 24, 24, color_temp);
    display.drawInvertedBitmap(0      , ab_temp-20, bitmap_temp, 24, 24, color_temp);
    display.drawInvertedBitmap(0      , ab_hum -20, bitmap_hum , 24, 24, color_hum);
    display.drawInvertedBitmap(0      , ab_iaq -20, bitmap_aiq , 24, 24, color_aiq);
    display.drawInvertedBitmap(0      , ab_co2 -14, bitmap_CO2 , 24, 24, color_co2);

    display.setRotation(1); //0-3
    display.setFont(&BabelSans9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(140, 14);
    display.print(epd_time);

    display.setFont(&BabelSans9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(200, 14);
    display.print(epd_date);

    display.setCursor(140+20, 32);
    display.setFont(&BabelSans8pt7b);
    display.print(epd_name);
    display.drawInvertedBitmap(140  , 32-14, bitmap_turtle, 18, 18, GxEPD_RED);

    display.hibernate();
    display.display(false);
}

void epd_s(String epd_name, String epd_date, String epd_time, String data_temp, String data_humidity, String data_airq, String data_co2  )
{
    display.init(9600);

    uint16_t color_temp = GxEPD_BLACK;
    uint16_t color_hum = GxEPD_BLACK;
    uint16_t color_aiq = GxEPD_BLACK;
    uint16_t color_co2 = GxEPD_BLACK;

    // Lines
    int line0 = 0;
    int line1 = 32;
    int line2 = 104;

    //Abstand
    int ab_temp = 35;
    int ab_hum  = ab_temp + 45;
    int ab_iaq  = ab_hum  + 45;
    int ab_co2  = ab_iaq  + 45;

    //Limit
    if ((data_temp.toDouble())    < 26)   { color_temp = GxEPD_BLACK;} else { color_temp = GxEPD_RED;}
    if ((data_humidity.toDouble())< 70)   { color_hum  = GxEPD_BLACK;} else { color_hum  = GxEPD_RED;}
    if ((data_airq.toDouble())    < 300)  { color_aiq  = GxEPD_BLACK;} else { color_aiq  = GxEPD_RED;}
    if ((data_co2.toDouble())     < 1500) { color_co2  = GxEPD_BLACK;} else { color_co2  = GxEPD_RED;}


    display.fillScreen(GxEPD_WHITE); // set the background to white (fill the buffer with value for white)
    display.setFullWindow();
    display.setCursor(0, 0);

    display.setRotation(2); //0-3
    display.setFont(&Inter_Bold12pt7b);

    display.setTextColor(color_temp);
    display.setCursor(line1, ab_temp);
    display.print(before_comma(data_temp));
    display.setFont(&Inter_Bold10pt7b);
    display.print(".");
    display.print(after_comma(data_temp));
    display.drawInvertedBitmap(line2-1, ab_temp-10, bitmap_grad18, 18, 18, color_temp);


    display.setFont(&Inter_Bold12pt7b);
    display.setTextColor(color_hum);
    display.setCursor(line1, ab_hum );
    display.print(before_comma(data_humidity));
    display.setFont(&Inter_Bold10pt7b);
    display.print(".");
    display.print(after_comma(data_humidity));


    display.setFont(&Inter_Bold12pt7b);
    display.setTextColor(color_aiq);
    display.setCursor(line1, ab_iaq );
    display.print(before_comma(data_airq));
    display.setFont(&Inter_Bold10pt7b);
    display.print(".");
    display.print(after_comma(data_airq));


    display.setFont(&Inter_Bold12pt7b);
    display.setTextColor(color_co2);
    display.setCursor(line1, ab_co2 + 5);
    display.print(data_co2);



    display.setFont(&Inter_Regular8pt7b);
    display.setTextColor(color_hum);
    display.setCursor(line2, ab_hum);
    display.print("%");

    display.setTextColor(color_aiq);
    display.setCursor(line2, ab_iaq);
    display.drawInvertedBitmap(line2  , ab_iaq-10, bitmap_iaq , 18, 18, color_aiq);

    display.drawInvertedBitmap(line2, ab_co2-10, bitmap_ppm18, 18, 18, color_co2);

    display.drawInvertedBitmap(line0  , ab_temp-20, bitmap_temp, 24, 24, color_temp);
    display.drawInvertedBitmap(line0  , ab_hum -20, bitmap_hum , 24, 24, color_hum);
    display.drawInvertedBitmap(line0  , ab_iaq -20, bitmap_aiq , 24, 24, color_aiq);
    display.drawInvertedBitmap(line0  , ab_co2 -14, bitmap_CO2 , 24, 24, color_co2);


    display.setRotation(2); //0-3
    display.setTextColor(GxEPD_BLACK);

    display.setFont(&BabelSans8pt7b);
    display.setCursor(line0, 228);
    display.print(epd_date);

    display.setFont(&BabelSans10pt7b);
    display.setCursor(line0+80, 228);
    display.print(epd_time);

    display.setFont(&BabelSans8pt7b);
    display.setCursor(line0+20, 248);
    display.print(epd_name);
    display.drawInvertedBitmap(line0  , 248-14, bitmap_turtle, 18, 18, GxEPD_RED);

    display.hibernate();
    display.display(false);
}


// --------------------------------------------------------------------------
// MAIN
// --------------------------------------------------------------------------
void setup()
{
    delay(100);
    Serial.begin(BAUDRATE);
    Serial.println();
    WiFiHandler::initWifi();
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
    unsigned long currentSeconds = millis() / 1000;

    BME680Handler &bmehandler = BME680Handler::getInstance();
    if (bmehandler.updateSensorData(currentSeconds)) {
        bmehandler.printout();
    }
    Bsec data = bmehandler.getData();

    // MHZ19
    MHZ19Handler &mhz19Handler = MHZ19Handler::getInstance();
    if (mhz19Handler.runUpdate(currentSeconds)) {
        mhz19Handler.printoutLastReadout();
    }

    // WiFi
    WiFiHandler::checkWifi(currentSeconds);

    // EPD
    currtimer_EPD = millis();
    if (currtimer_EPD - prevtimer_EPD >= interval_EPD) {
        Serial.print("EPD: updated");
        epd_s(DeviceName,
              localTime("%Y.%m.%d"),
              localTime("%H:%M"),
              String(data.temperature),
              String(data.humidity),
              String(data.iaq),
              String(mhz19Handler.getLastReadout().getRegular()));
        prevtimer_EPD = currtimer_EPD;
    }

    // RAM
    currtimer_RAM = millis();
    if (currtimer_RAM - prevtimer_RAM >= interval_RAM) {
        Serial.print("Memory Usage: ");
        uint32_t freeHeap = ESP.getFreeHeap();
        uint32_t maximumHeap = ESP.getHeapSize();
        uint32_t usedHeap = maximumHeap - freeHeap;
        Serial.print(usedHeap);
        Serial.print("b | ");
        Serial.print(maximumHeap);
        Serial.println("b");
        prevtimer_RAM = currtimer_RAM;
    }
}