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


//Sensor
#include "bsec.h"

#include <SoftwareSerial.h>
#include "MHZ19.h"

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
#define BAUDRATE 9600
String consolOUT;

// sensor MH-Z19B
#define PIN_MHZ19_RX 17
#define PIN_MHZ19_TX 16


// WLAN
const char *ssid     = WIFI_SSID;
const char *password = WIFI_PW;
String deviceName    = "SensorTurtle 1";

// timezone
const String timezone = "CET-1CEST,M3.5.0,M10.5.0/3";

// EPD
// in separate file
#include "GxEPD2_display_selection_new_style.h"

// ---------------------------------------------
// Declaration
// ---------------------------------------------


// Timer
const long interval_MHZ19  = 60000*0.5;
const long interval_WIFI   = 60000*1;
const long interval_EPD    = 60000*3;
const long interval_RAM    = 60000*0.5;

unsigned long prevtimer_MHZ19  = 0;
unsigned long prevtimer_WIFI   = 0;
unsigned long prevtimer_EPD    = 0;
unsigned long prevtimer_RAM    = 0;

unsigned long currtimer_MHZ19  = 0;
unsigned long currtimer_WIFI   = 0;
unsigned long currtimer_EPD    = 0;
unsigned long currtimer_RAM    = 0;

// --------------------------------------------------------------------------
// sensor data
// --------------------------------------------------------------------------
const String name_MHZ19_co2               = "MHZ19 CO2 [ppm]";
const String name_MHZ19_co2_raw           = "MHZ19 CO2 raw [ppm]";
const String name_MHZ19_co2_unlimited     = "MHZ19 unlimited";
const String name_MHZ19_co2_limited       = "MHZ19 limited";
const String name_MHZ19_co2_background    = "MHZ19 Background";
const String name_MHZ19_co2_tempAdjust    = "MHZ19 Temperature Adjust";
const String name_MHZ19_co2_temperatur    = "MHZ19 Temperature [C]";
const String name_MHZ19_co2_Accuracy      = "MHZ19 Accuracy";
const String name_MHZ19_timestamp         = "MHZ19 Timestamp [ms]";
const String name_MHZ19_datetime          = "MHZ19 Date and Time";
const String name_MHZ19_date              = "MHZ19 Date";
const String name_MHZ19_time              = "MHZ19 Time";
const String name_MHZ19_zone              = "MHZ19 Timezone";

String data_MHZ19_timestamp        = "";
String data_MHZ19_co2              = "";
String data_MHZ19_co2_raw          = "";
String data_MHZ19_co2_unlimited    = "";
String data_MHZ19_co2_limited      = "";
String data_MHZ19_co2_background   = "";
String data_MHZ19_co2_tempAdjust   = "";
String data_MHZ19_co2_temperatur   = "";
String data_MHZ19_co2_Accuracy     = "";
String data_MHZ19_date             = "";
String data_MHZ19_time             = "";
String data_MHZ19_zone             = "";
String data_MHZ19_datetime         = "";

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
// BME680 sensor
// --------------------------------------------------------------------------

//save calibration data
#define STATE_SAVE_PERIOD UINT32_C(1440 * 60 * 1000) // 1440 minutes - 1 times a day
uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
uint16_t stateUpdateCounter = 0;

// --------------------------------------------------------------------------
// MH-Z19 sensor Co2
// --------------------------------------------------------------------------
MHZ19 myMHZ19;
SoftwareSerial Serial_MHZ19(PIN_MHZ19_RX, PIN_MHZ19_TX);
unsigned long getDataTimer = 0;
void setRange(int range);

void MHZ19setup(){

    Serial_MHZ19.begin(BAUDRATE);                                // Uno Example: Begin Stream with MHZ19 baudrate
    myMHZ19.begin(Serial_MHZ19);                                 // *Important, Pass your Stream reference

    delay(200);
    //myMHZ19.printCommunication();                            // Error Codes are also included here if found (mainly for debugging/interest)

    myMHZ19.autoCalibration(true);
    Serial.print("MHZ19: ABC Status: "); myMHZ19.getABC() ? Serial.println("ON") :  Serial.println("OFF");  // now print it's status

    char myVersion[4];
    myMHZ19.getVersion(myVersion);

    Serial.print("MHZ19: Range: ");
    Serial.println(myMHZ19.getRange());

    //Serial.println(""MHZ19: Calibrating..");
    //myMHZ19.calibrate();    // Take a reading which be used as the zero point for 400 ppm^

    myMHZ19.verify();

    Serial.println("");
}

void MHZ19setValues(){
    if(myMHZ19.errorCode == RESULT_OK)
    {
      data_MHZ19_timestamp      = String(millis());
      data_MHZ19_datetime       = localTime("%Y.%m.%d %H:%M");
      data_MHZ19_date           = localTime("%Y.%m.%d");
      data_MHZ19_time           = localTime("%H:%M");
      data_MHZ19_zone           = localTime("%Z %z");
      data_MHZ19_co2            = String(myMHZ19.getCO2());
      data_MHZ19_co2_raw        = String(myMHZ19.getCO2Raw());
      data_MHZ19_co2_unlimited  = String(myMHZ19.getCO2(true, true));
      data_MHZ19_co2_limited    = String(myMHZ19.getCO2(false, true));
      data_MHZ19_co2_background = String(myMHZ19.getBackgroundCO2());
      data_MHZ19_co2_tempAdjust = String(myMHZ19.getTempAdjustment());
      data_MHZ19_co2_Accuracy   = String(myMHZ19.getAccuracy());
      data_MHZ19_co2_temperatur = String(myMHZ19.getTemperature());
    }
    else
    {
      Serial.println("Failed to recieve CO2 value - Error");
      Serial.print("Response Code: ");
      Serial.println(myMHZ19.errorCode);          // Get the Error Code value
    }

}

void MHZ19printout(){
  consolOUT = "MHZ19: \n";
  consolOUT += name_MHZ19_timestamp       + ":     "       + data_MHZ19_timestamp     + ", \n";
  consolOUT += name_MHZ19_datetime        + ":      "      + data_MHZ19_datetime      + ", \n";
  consolOUT += name_MHZ19_co2             + ":          "  + data_MHZ19_co2           + ", \n";
  consolOUT += name_MHZ19_co2_unlimited   + ":          "  + data_MHZ19_co2_unlimited + ", \n";
  consolOUT += name_MHZ19_co2_limited     + ":            "+ data_MHZ19_co2_limited   + ", \n";
  consolOUT += name_MHZ19_co2_background  + ":         "   + data_MHZ19_co2_background+ ", \n";
  consolOUT += name_MHZ19_co2_tempAdjust  + ": "           + data_MHZ19_co2_tempAdjust+ ", \n";
  consolOUT += name_MHZ19_co2_temperatur  + ":    "        + data_MHZ19_co2_temperatur+ ", \n";
  consolOUT += name_MHZ19_co2_Accuracy    + ":           " + data_MHZ19_co2_Accuracy  + ", \n";
  Serial.println(consolOUT);
  consolOUT = "";
}


// --------------------------------------------------------------------------
// WLAN functions
// --------------------------------------------------------------------------

void WiFiReStart()
{
  if (*ssid)
  {
    // Connect to WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);
    int wifiWaitCount = 0;
    while (WiFi.status() != WL_CONNECTED && wifiWaitCount < 20)
    {
      delay(250);
      wifiWaitCount++;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("");
      Serial.println("WiFi connected");

      // Print the IP address
      Serial.println(WiFi.localIP());
    }
  }
}

void WiFisetup()
{
  int wifiWaitCount = 0;
  WiFi.setHostname(deviceName.c_str());
  Serial.print("\nWIFI: Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED && wifiWaitCount < 20)
  {
    delay(250);
    wifiWaitCount++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WIFI: connected.");
    Serial.print("WIFI: IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("WIFI: not connected");
  }
}

void WiFiStatusCheck()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFiReStart();
  }
  else
  {
    Serial.println("WIFI: Still connected");
  }
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

  WiFisetup();
  MHZ19setup();

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

}

void loop() {

    BME680Handler &bmehandler = BME680Handler::getInstance();
    if (bmehandler.updateSensorData()){
        bmehandler.printout();
    }
    Bsec data = bmehandler.getData();
  // MHZ19
  currtimer_MHZ19 = millis();
  if (currtimer_MHZ19 - prevtimer_MHZ19 >= interval_MHZ19){
    MHZ19setValues();
    MHZ19printout();
    prevtimer_MHZ19 = currtimer_MHZ19;
  }

  // WiFi
  currtimer_WIFI = millis();
  if (currtimer_WIFI - prevtimer_WIFI >= interval_WIFI){
    WiFiStatusCheck();
    prevtimer_WIFI = currtimer_WIFI;
  }

  // EPD
  currtimer_EPD = millis();
  if (currtimer_EPD - prevtimer_EPD >= interval_EPD){
    Serial.print("EPD: updated");
    Serial.println(data_MHZ19_timestamp);
    epd_s(deviceName,
          localTime("%Y.%m.%d") ,
          localTime("%H:%M") ,
          String(data.temperature),
          String(data.humidity),
          String(data.iaq),
          data_MHZ19_co2 );
    prevtimer_EPD = currtimer_EPD;
  }

    // RAM
  currtimer_RAM = millis();
  if (currtimer_RAM - prevtimer_RAM >= interval_RAM){
   Serial.print("Memory Usage: ");
   Serial.println(ESP.getFreeHeap());
   prevtimer_RAM = currtimer_RAM;
  }
}