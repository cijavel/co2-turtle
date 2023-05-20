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
#include "bsec.h"
#include <ctime>
#include "EPDHandler.h"

// --------------------------------------------------------------------------
// time functions
// --------------------------------------------------------------------------
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;
const String timezone = "CET-1CEST,M3.5.0,M10.5.0/3";

String localTime(const String& format) {
    struct tm timeinfo{};

    String time = "";
    char toutp[60];
    setenv("TZ", timezone.c_str(), 1);  //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
    tzset();

    if (!getLocalTime(&timeinfo)) {
        time = "TIME: Failed to obtain";
    } else {
        strftime(toutp, sizeof(toutp), format.c_str(), &timeinfo);
        time = String(toutp);
    }
    return time;
}

//void epd_w(String epd_name, String epd_date, String epd_time, String data_temp, String data_humidity, String data_airq, String data_co2  )
//{
//    display.init(9600);
//
//    uint16_t color_temp = GxEPD_BLACK;
//    uint16_t color_hum = GxEPD_BLACK;
//    uint16_t color_aiq = GxEPD_BLACK;
//    uint16_t color_co2 = GxEPD_BLACK;
//
//    // Lines
//    int line1 = 30;
//    int line2 = 90;
//
//    //Abstand
//    int ab_temp = 20;
//    int ab_hum  = 52;
//    int ab_iaq  = 86;
//    int ab_co2  = 115;
//
//    //Limit
//    if ((data_temp.toDouble())    < 26)   { color_temp = GxEPD_BLACK;} else { color_temp = GxEPD_RED;}
//    if ((data_humidity.toDouble())< 70)   { color_hum  = GxEPD_BLACK;} else { color_hum  = GxEPD_RED;}
//    if ((data_airq.toDouble())    < 300)  { color_aiq  = GxEPD_BLACK;} else { color_aiq  = GxEPD_RED;}
//    if ((data_co2.toDouble())     < 1500) { color_co2  = GxEPD_BLACK;} else { color_co2  = GxEPD_RED;}
//
//
//    display.fillScreen(GxEPD_WHITE); // set the background to white (fill the buffer with value for white)
//    display.setFullWindow();
//    display.setCursor(0, 0);
//
//    display.setRotation(1); //0-3
//    display.setFont(&Inter_Bold12pt7b);
//
//    display.setTextColor(color_temp);
//    display.setCursor(line1, ab_temp);
//    display.print(before_comma(data_temp));
//    display.setFont(&Inter_Bold10pt7b);
//    display.print(".");
//    display.print(before_comma(data_temp));
//
//
//    display.setFont(&Inter_Bold12pt7b);
//    display.setTextColor(color_hum);
//    display.setCursor(line1, ab_hum );
//    display.print(before_comma(data_humidity));
//    display.setFont(&Inter_Bold10pt7b);
//    display.print(".");
//    display.print(before_comma(data_humidity));
//
//
//    display.setFont(&Inter_Bold12pt7b);
//    display.setTextColor(color_aiq);
//    display.setCursor(line1, ab_iaq );
//    display.print(before_comma(data_airq));
//    display.setFont(&Inter_Bold10pt7b);
//    display.print(".");
//    display.print(before_comma(data_airq));
//
//
//    display.setFont(&Inter_Bold12pt7b);
//    display.setTextColor(color_co2);
//    display.setCursor(line1, ab_co2 + 5);
//    display.print(data_co2);
//
//
//    display.setFont(&Inter_Regular10pt7b);
//
//
//    display.setTextColor(color_hum);
//    display.setCursor(line2, ab_hum);
//    display.print(" %");
//
//    display.setTextColor(color_aiq);
//    display.setCursor(line2, ab_iaq);
//    display.print(" IAQ");
//
//    display.setTextColor(color_co2);
//    display.setCursor(line2, ab_co2+2 );
//    display.print(" ppm");
//
//    display.drawInvertedBitmap(line2+5, ab_temp-20, bitmap_grad, 24, 24, color_temp);
//    display.drawInvertedBitmap(0      , ab_temp-20, bitmap_temp, 24, 24, color_temp);
//    display.drawInvertedBitmap(0      , ab_hum -20, bitmap_hum , 24, 24, color_hum);
//    display.drawInvertedBitmap(0      , ab_iaq -20, bitmap_aiq , 24, 24, color_aiq);
//    display.drawInvertedBitmap(0      , ab_co2 -14, bitmap_CO2 , 24, 24, color_co2);
//
//    display.setRotation(1); //0-3
//    display.setFont(&BabelSans9pt7b);
//    display.setTextColor(GxEPD_BLACK);
//    display.setCursor(140, 14);
//    display.print(epd_time);
//
//    display.setFont(&BabelSans9pt7b);
//    display.setTextColor(GxEPD_BLACK);
//    display.setCursor(200, 14);
//    display.print(epd_date);
//
//    display.setCursor(140+20, 32);
//    display.setFont(&BabelSans8pt7b);
//    display.print(epd_name);
//    display.drawInvertedBitmap(140  , 32-14, bitmap_turtle, 18, 18, GxEPD_RED);
//
//    display.hibernate();
//    display.display(false);
//}

#ifdef DEBUG
static void PrintRamUsage(unsigned long currentSeconds) {
    if (currentSeconds % interval_RAMPrintout_in_Seconds == 0) {
        Serial.print("Memory Usage: ");
        uint32_t freeHeap = ESP.getFreeHeap();
        uint32_t maximumHeap = ESP.getHeapSize();
        uint32_t usedHeap = maximumHeap - freeHeap;
        Serial.print(usedHeap);
        Serial.print("b | ");
        Serial.print(maximumHeap);
        Serial.println("b");
    }
}
#endif

void setup() {
    delay(100);
    Serial.begin(BAUDRATE);
    Serial.println();
    WiFiHandler::initWifi();
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}
unsigned long last = 0;
void loop() {
    unsigned long currentSeconds = millis() / 1000;
#ifdef DEBUG
    if (currentSeconds != last) {
        Serial.print("Current loop second: ");
        Serial.println(currentSeconds);
        last = currentSeconds;
    }
#endif
    BME680Handler &bmehandler = BME680Handler::getInstance();
#ifdef DEBUG
    if (bmehandler.updateSensorData(currentSeconds)) {
        bmehandler.printout();
    }
#else
    bmehandler.updateSensorData(currentSeconds);
#endif
    Bsec bme_data = bmehandler.getData();
    MHZ19Handler &mhz19Handler = MHZ19Handler::getInstance();
#ifdef DEBUG
    if (mhz19Handler.runUpdate(currentSeconds)) {
        mhz19Handler.printoutLastReadout();
    }
#else
    mhz19Handler.runUpdate(currentSeconds);
#endif

    WiFiHandler::checkWifi(currentSeconds);

    EPDHandler::updateEPDvertical(mhz19Handler.getLastReadout(), bme_data, localTime("%Y.%m.%d"), localTime("%H:%M"),
                                  currentSeconds);
#ifdef DEBUG
    PrintRamUsage(currentSeconds);
#endif
}