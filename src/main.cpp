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
#include "WebServerHandler.h"
#include "FastLedHandler.h"


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
    WebServerHandler &webServer = WebServerHandler::getInstance();
    webServer.start();

    FastLedHandler &ledhandler = FastLedHandler::getInstance();
    ledhandler.addLEDsection();
    ledhandler.setup_led();
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


    CO2Data mhz19Readout = mhz19Handler.getLastReadout();
    WiFiHandler::checkWifi(currentSeconds);


    WebServerHandler &webServer = WebServerHandler::getInstance();
    webServer.setInputDataforBody(mhz19Readout, bme_data);


    EPDHandler::updateEPDvertical(mhz19Readout, bme_data, localTime("%Y.%m.%d"), localTime("%H:%M"), currentSeconds);

    FastLedHandler &ledHandler = FastLedHandler::getInstance();
    ledHandler.setInputDataforLED(mhz19Readout, bme_data);
    ledHandler.ledstatus(currentSeconds);


#ifdef DEBUG
    PrintRamUsage(currentSeconds);
#endif
}