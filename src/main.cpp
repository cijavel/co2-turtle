#include <Arduino.h>
#include "bsec.h"
#include "MHZ19.h"
#include <FastLED.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SectionManager.h>
#include <helpers.h>


//https://github.com/cyberman54/ESP32-Paxcounter/blob/master/src/bmesensor.cpp
//https://www.rehva.eu/fileadmin/user_upload/REHVA_COVID-19_guidance_document_V3_03082020.pdf

//Senor
//https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME680-DS001.pdf
//https://www.winsen-sensor.com/d/files/infrared-gas-sensor/ndir-co2-sensor/mh-z19b-co2-manual(ver1_6).pdf

//Libs
//https://github.com/FastLED/FastLED/wiki/Basic-usage
//https://github.com/chris-schmitz/FastLED-Section-Manager?utm_source=platformio&utm_medium=piohome

// ---------------------------------------------
// Configuration
// ---------------------------------------------


// WLAN
const char *ssid     = "";
const char *password = "";
String deviceName    = "ESP32 SensorTurtle";



// ---------------------------------------------
// Helper functions declarations
// ---------------------------------------------
void checkIaqSensorStatus(void);
void errLeds(void);
void loadState(void);
void updateState(void);
void clearState(void);


// --------------------------------------------------------------------------
// sensor data
// --------------------------------------------------------------------------

const uint8_t bsec_config_iaq[] = {  
  //#include "config/generic_33v_300s_28d/bsec_iaq.txt"
  #include "config/generic_33v_300s_4d/bsec_iaq.txt"
};
//save calibration data
#define STATE_SAVE_PERIOD UINT32_C(720 * 60 * 1000) // 720 minutes - 2 times a day
MHZ19 myMHZ19; // Co2 sensor


// Create an object of the class Bsec
Bsec iaqSensor;
uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
uint16_t stateUpdateCounter = 0;
uint32_t millisOverflowCounter = 0;
uint32_t lastTime = 0;
int latest_accuracy = 0;

const String name_timestamp           = "Timestamp [ms]";
const String name_rawtemperatur       = "raw temperature [°C]";
const String name_pressure            = "pressure [hPa]";
const String name_rawrelativehumidity = "raw relative humidity [%]";
const String name_gas                 = "gas [Ohm]";
const String name_iaq                 = "IAQ";
const String name_iaqaccuracy         = "IAQ accuracy";
const String name_temp                = "temperature [°C]";
const String name_relativehumidity    = "relative humidity [%]";
const String name_iaqstatic           = "IAQ Static";
const String name_co2equil            = "CO2 equivalentv";
const String name_breahtvoc           = "breath VOC equivalent";
const String name_MHZ19B_co2          = "MHZ19B CO2 [ppm]";

String data_timestamp           = "";
String data_rawtemperatur       = "";
String data_pressure            = "";
String data_rawrelativehumidity = "";
String data_gas                 = "";
String data_iaq                 = "";
String data_iaqaccuracy         = "";
String data_temp                = "";
String data_relativehumidity    = "";
String data_iaqstatic           = "";
String data_co2equil            = "";
String data_breahtvoc           = "";
String data_MHZ19B_co2          = "";

String header_data = "";
String output      = "";

const String header =   
  name_timestamp           + ", " + 
  name_rawtemperatur       + ", " + 
  name_temp                + ", " + 
  name_pressure            + ", " + 
  name_rawrelativehumidity + ", " + 
  name_relativehumidity    + ", " + 
  name_gas                 + ", " + 
  name_iaq                 + ", " + 
  name_iaqaccuracy         + ", " + 
  name_iaqstatic           + ", " +  
  name_co2equil            + ", " + 
  name_breahtvoc           + ", " + 
  name_MHZ19B_co2 ;


// Helper function definitions
void checkIaqSensorStatus(void)
{
  if (iaqSensor.status != BSEC_OK)
  {
    if (iaqSensor.status < BSEC_OK)
    {
      output = "BSEC error code : " + String(iaqSensor.status);
      Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    }
    else
    {
      output = "BSEC warning code : " + String(iaqSensor.status);
      Serial.println(output);
    }
  }

  if (iaqSensor.bme680Status != BME680_OK)
  {
    if (iaqSensor.bme680Status < BME680_OK)
    {
      output = "BME680 error code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    }
    else
    {
      output = "BME680 warning code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
    }
  }
}


// --------------------------------------------------------------------------
// definition of LED
// --------------------------------------------------------------------------
#define LED_WLANCONNECT 0
#define LED_STATUS 1
#define LED_TEMP 2
#define LED_HUM 3
#define LED_AIRQ 4
#define LED_CO2 5


#define NUM_LEDS 34
#define DATA_PIN 5
CRGB led[NUM_LEDS];
SectionManager LEDsectionManager = SectionManager(led);
int ledloop = 0;

void addLEDsection(void)
{
  LEDsectionManager.addSections(6);
  LEDsectionManager.addRangeToSection(LED_WLANCONNECT, 0, 0,  false); // LED_WLAN
  LEDsectionManager.addRangeToSection(LED_STATUS,      1, 1,  false); // LED_STATUS data Quality
  LEDsectionManager.addRangeToSection(LED_TEMP,        3, 9,  false); // LED_TEMP
  LEDsectionManager.addRangeToSection(LED_HUM,        11, 17, false); // LED_HUM
  LEDsectionManager.addRangeToSection(LED_AIRQ,       19, 25, false); // LED_AIRQ
  LEDsectionManager.addRangeToSection(LED_CO2,        27, 33, false); // LED_CO2

    //FastLED.addLeds<NEOPIXEL, DATA_PIN>(led, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(led, NUM_LEDS);
  FastLED.clear(true);

}


int rainbowAllSections(uint8_t pauseDuration, uint16_t wheelPosition, int multi)
{
    if (NUM_LEDS * multi < wheelPosition)
    {
      wheelPosition = 0 ;
    }
   wheelPosition += 1;


  int colorsteps = 230; // how many colors, 256 all color, 
  int colors = 9 ;  // circle Abstand zwischen den Farben pro takt. je höher desto feiner
  uint16_t level ;
  // for (level = 0; level > LEDsectionManager.getTotalLevels(); level++) // gegen uhrzeiger einblenden
  for (level = LEDsectionManager.getTotalLevels(); level > 0 ; level--)   // mit uhrzeiger einblenden
  {
    uint32_t color = Wheel((level * colors + wheelPosition) & colorsteps); 

    for (uint8_t i = LEDsectionManager.getTotalLevels(); i > 0; i--) // mit uhrzeiger farbe ändern
    {
      LEDsectionManager.setColorAtGlobalIndex(level, color);
    }

    FastLED.setBrightness(50);
    FastLED.show();
    delay(pauseDuration);
  }
  return wheelPosition;
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
      Serial.print(".");
      wifiWaitCount++;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("");
      Serial.println("WiFi connected");

      // Start the server
      //server.begin();

      //Serial.println("Server started");

      // Print the IP address
      Serial.println(WiFi.localIP());
    }
  }
}

void WiFiSetup()
{
  int wifiWaitCount = 0;
  WiFi.setHostname(deviceName.c_str());
  Serial.print("\nConnecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED && wifiWaitCount < 20)
  {
    delay(800);
    Serial.print(".");
    wifiWaitCount++;
    LEDsectionManager.fillSectionWithColor(0, CRGB::DarkBlue, FillStyle(ALL_AT_ONCE));
    FastLED.setBrightness(wifiWaitCount * 2);
    FastLED.show();
  }
  // Print local IP address and start web server
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    LEDsectionManager.fillSectionWithColor(0, CRGB::DarkGreen, FillStyle(ALL_AT_ONCE));
    FastLED.show();
  }
  else
  {
    Serial.println("WiFi not connected");
    LEDsectionManager.fillSectionWithColor(0, CRGB::DarkRed, FillStyle(ALL_AT_ONCE));
    FastLED.show();
  }
  FastLED.clear(true);
}




// --------------------------------------------------------------------------
// Web Server
// --------------------------------------------------------------------------
AsyncWebServer server(80);

#define RX_PIN 18
#define TX_PIN 19
#define BAUDRATE 9600

HardwareSerial mySerial(1);

void handle_NotFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain; charset=utf-8", "Not found");
}

void handle_data(AsyncWebServerRequest *request)
{
  header_data =
  "{\n\"" + 
  name_timestamp           + "\":\"" + data_timestamp           + "\",\n" + "\"" +
  name_rawtemperatur       + "\":\"" + data_rawtemperatur       + "\",\n" + "\"" +
  name_temp                + "\":\"" + data_temp                + "\",\n" + "\"" + 
  name_pressure            + "\":\"" + data_pressure            + "\",\n" + "\"" +
  name_rawrelativehumidity + "\":\"" + data_rawrelativehumidity + "\",\n" + "\"" + 
  name_relativehumidity    + "\":\"" + data_relativehumidity    + "\",\n" + "\"" + 
  name_gas                 + "\":\"" + data_gas                 + "\",\n" + "\"" + 
  name_iaq                 + "\":\"" + data_iaq                 + "\",\n" + "\"" + 
  name_iaqaccuracy         + "\":\"" + data_iaqaccuracy         + "\",\n" + "\"" + 
  name_iaqstatic           + "\":\"" + data_iaqstatic           + "\",\n" +  "\"" + 
  name_co2equil            + "\":\"" + data_co2equil            + "\",\n" + "\"" +
  name_breahtvoc           + "\":\"" + data_breahtvoc           + "\",\n" + "\"" +
  name_MHZ19B_co2          + "\":\"" + data_MHZ19B_co2          + "\"\n}";
  request->send(200, "application/json; charset=utf-8", header_data);
}

void handle_data_only(AsyncWebServerRequest *request)
{
  request->send(200, "text/plain; charset=utf-8", output);
}

void mh_z19b_calibrateZero(AsyncWebServerRequest *request)
{
  if (request->hasParam("calibrateZero"))
  {
    if (request->getParam("calibrateZero")->value() == "true")
    {
      myMHZ19.calibrate();
      request->send(200, "text/plain; charset=utf-8", output);
    }
    else
    {
      request->send(200, "text/plain; charset=utf-8", "Missing Get Param Value ?calibrateZero=true");
    }
  }
  else
  {
    request->send(200, "text/plain; charset=utf-8", "Missing Get Param ?calibrateZero=true");
  }
}



// --------------------------------------------------------------------------
//  MAIN
// --------------------------------------------------------------------------
// Entry point for the example
void setup(void)
{
  addLEDsection();

  Serial.begin(9600);

  WiFiSetup();


  // webpages on server
  server.on("/", HTTP_GET, handle_data);
  server.on("/dataonly", HTTP_GET, handle_data_only);
  server.on("/CO2", HTTP_GET, mh_z19b_calibrateZero);
  server.onNotFound(handle_NotFound);

  server.begin();

  EEPROM.begin(BSEC_MAX_STATE_BLOB_SIZE + 1);           // 1st address for the length
  mySerial.begin(BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN); // ESP32 Example
  myMHZ19.begin(mySerial);                              // *Important, Pass your Stream reference
  myMHZ19.autoCalibration(true);                        // Turn Auto Calibration OFF
  Wire.begin();

  iaqSensor.begin(0x77, Wire);
  output = "\nBSEC library version " + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
  Serial.println(output);
  checkIaqSensorStatus();

  iaqSensor.setTemperatureOffset(4);

  iaqSensor.setConfig(bsec_config_iaq);
  checkIaqSensorStatus();
 // clearState();
  loadState();

  bsec_virtual_sensor_t sensorList[10] = {
      BSEC_OUTPUT_RAW_TEMPERATURE,
      BSEC_OUTPUT_RAW_PRESSURE,
      BSEC_OUTPUT_RAW_HUMIDITY,
      BSEC_OUTPUT_RAW_GAS,
      BSEC_OUTPUT_IAQ,
      BSEC_OUTPUT_STATIC_IAQ,
      BSEC_OUTPUT_CO2_EQUIVALENT,
      BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
      BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
      BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
  };

  iaqSensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_ULP);
  checkIaqSensorStatus();
  
  Serial.println(header); 
}

// Function that is looped forever
void loop(void)
{
  

  if (WiFi.status() != WL_CONNECTED)
  {
    LEDsectionManager.fillSectionWithColor(LED_WLANCONNECT, CRGB::DarkRed, FillStyle(ALL_AT_ONCE));
    FastLED.show();
    delay(150);

    LEDsectionManager.fillSectionWithColor(LED_WLANCONNECT, CRGB::Black , FillStyle(ALL_AT_ONCE)); 
    FastLED.show();
    WiFiReStart();
    delay(1000);
  }
  else
  {
    LEDsectionManager.fillSectionWithColor(LED_WLANCONNECT, CRGB::LightSkyBlue, FillStyle(ALL_AT_ONCE));
    FastLED.show();
    delay(150);

    LEDsectionManager.fillSectionWithColor(LED_WLANCONNECT, CRGB::Black , FillStyle(ALL_AT_ONCE)); 
    FastLED.show();
    delay(1000);
  }


  if (iaqSensor.iaqAccuracy == 0)
  {
    ledloop = rainbowAllSections(20, ledloop, 255);
  }
  unsigned long time_trigger = millis();
  if (iaqSensor.run())
  { // If new data is available

		data_timestamp           = String(time_trigger);
		data_rawtemperatur       = String(iaqSensor.rawTemperature);
		data_pressure            = String(iaqSensor.pressure / 100.0);
		data_rawrelativehumidity = String(iaqSensor.rawHumidity);
		data_gas                 = String(iaqSensor.gasResistance);
		data_iaq                 = String(iaqSensor.iaq);
		data_iaqaccuracy         = String(iaqSensor.iaqAccuracy);
		data_temp                = String(iaqSensor.temperature);
		data_relativehumidity    = String(iaqSensor.humidity);
		data_iaqstatic           = String(iaqSensor.staticIaq);
		data_co2equil            = String(iaqSensor.co2Equivalent);
		data_breahtvoc           = String(iaqSensor.breathVocEquivalent);
    data_MHZ19B_co2          = String(myMHZ19.getCO2());

    output = String(time_trigger);
    output += ", " + String(iaqSensor.rawTemperature);
    output += ", " + String(iaqSensor.pressure);
    output += ", " + String(iaqSensor.rawHumidity);
    output += ", " + String(iaqSensor.gasResistance);
    output += ", " + String(iaqSensor.iaq);
    output += ", " + String(iaqSensor.iaqAccuracy);


    if (iaqSensor.iaqAccuracy == 0)
    {

    }
    else if (iaqSensor.iaqAccuracy == 1)
    {
      LEDsectionManager.fillSectionWithColor(LED_STATUS, CRGB::Yellow, FillStyle(ALL_AT_ONCE));
    }
    else if (iaqSensor.iaqAccuracy == 2)
    {
      LEDsectionManager.fillSectionWithColor(LED_STATUS, CRGB::Green, FillStyle(ALL_AT_ONCE));
    }
    else if (iaqSensor.iaqAccuracy >= 3)
    {
      LEDsectionManager.fillSectionWithColor(LED_STATUS, CRGB::Green, FillStyle(ALL_AT_ONCE));
      updateState(); //acurate data. save them
    }





    output += ", " + String(iaqSensor.temperature);
    if (iaqSensor.iaqAccuracy > 0)
    {
      if (iaqSensor.temperature < 19)
      {
        LEDsectionManager.fillSectionWithColor(LED_TEMP, CRGB::Magenta, FillStyle(ALL_AT_ONCE));
      }
      else if (iaqSensor.temperature < 20)
      {
        LEDsectionManager.fillSectionWithColor(LED_TEMP, CRGB::SeaGreen, FillStyle(ALL_AT_ONCE));
      }
      else if (iaqSensor.temperature > 26)
      {
        LEDsectionManager.fillSectionWithColor(LED_TEMP, CRGB::Orange, FillStyle(ALL_AT_ONCE));
      }
      else if (iaqSensor.temperature > 28)
      {
        LEDsectionManager.fillSectionWithColor(LED_TEMP, CRGB::Red, FillStyle(ALL_AT_ONCE));
      }
      else
      {
        LEDsectionManager.fillSectionWithColor(LED_TEMP, CRGB::Green, FillStyle(ALL_AT_ONCE));
      }
    }
    else
    {
      //LEDsectionManager.fillSectionWithColor(LED_TEMP, CRGB::Black, FillStyle(ALL_AT_ONCE));
    }
    




    output += ", " + String(iaqSensor.humidity);
    if (iaqSensor.iaqAccuracy > 0)
    {
      if (iaqSensor.humidity < 20) // Far too dry
      {
        LEDsectionManager.fillSectionWithColor(LED_HUM, CRGB::Red, FillStyle(ALL_AT_ONCE));
      }
      else if (iaqSensor.humidity < 30) // Too dry
      {
        LEDsectionManager.fillSectionWithColor(LED_HUM, CRGB::Yellow, FillStyle(ALL_AT_ONCE)); 
      }
      else if (iaqSensor.humidity < 40) // dry
      {
        LEDsectionManager.fillSectionWithColor(LED_HUM, CRGB::GreenYellow, FillStyle(ALL_AT_ONCE)); 
      }
      else if (iaqSensor.humidity < 50) //normal
      {
        LEDsectionManager.fillSectionWithColor(LED_HUM, CRGB::Green, FillStyle(ALL_AT_ONCE)); 
      }
      else if (iaqSensor.humidity < 60) // Slightly moist
      {
        LEDsectionManager.fillSectionWithColor(LED_HUM, CRGB::YellowGreen, FillStyle(ALL_AT_ONCE)); 
      }
      else if (iaqSensor.humidity < 65) // moist
      {
        LEDsectionManager.fillSectionWithColor(LED_HUM, CRGB::Orange, FillStyle(ALL_AT_ONCE)); 
      }
      else if (iaqSensor.humidity < 70) // very moist
      {
        LEDsectionManager.fillSectionWithColor(LED_HUM, CRGB::Red, FillStyle(ALL_AT_ONCE)); 
      }
      else // wet
      {
        LEDsectionManager.fillSectionWithColor(LED_HUM, CRGB::Black, FillStyle(ALL_AT_ONCE));
        FastLED.show();
        delay(150);

        LEDsectionManager.fillSectionWithColor(LED_HUM, CRGB::Magenta , FillStyle(ALL_AT_ONCE)); 
        FastLED.show();
        delay(500);
      }
    }
    else
    {
         //LEDsectionManager.fillSectionWithColor(LED_HUM, CRGB::Black, FillStyle(ALL_AT_ONCE)); 
    }




    output += ", " + String(iaqSensor.staticIaq);
    if (iaqSensor.iaqAccuracy > 0)
    {
      if (iaqSensor.iaq <= 50) // exellent
      {  
        LEDsectionManager.fillSectionWithColor(LED_AIRQ, CRGB::SeaGreen, FillStyle(ALL_AT_ONCE)); 
      }
      else if (iaqSensor.iaq <= 100) // good
      {  
        LEDsectionManager.fillSectionWithColor(LED_AIRQ, CRGB::Green, FillStyle(ALL_AT_ONCE)); 
      }
      else if (iaqSensor.iaq <= 150) // lightly polluted. Ventilation suggested.
      {  
        LEDsectionManager.fillSectionWithColor(LED_AIRQ, CRGB::YellowGreen, FillStyle(ALL_AT_ONCE)); 
      }
      else if (iaqSensor.iaq <= 200) // moderately polluted. please ventilate.
      {  
        LEDsectionManager.fillSectionWithColor(LED_AIRQ, CRGB::Yellow, FillStyle(ALL_AT_ONCE));
      }
      else if (iaqSensor.iaq < 250) // heavily polluted. please ventilate.
      {  
        LEDsectionManager.fillSectionWithColor(LED_AIRQ, CRGB::Orange, FillStyle(ALL_AT_ONCE)); 
      }
      else if (iaqSensor.iaq < 300) // severly polluted. please ventilate urgently.
      {  
        LEDsectionManager.fillSectionWithColor(LED_AIRQ, CRGB::Red, FillStyle(ALL_AT_ONCE)); 
      }
      else // extremly polluted. please ventilate urgently.
      {  
        LEDsectionManager.fillSectionWithColor(LED_AIRQ, CRGB::Black, FillStyle(ALL_AT_ONCE));
        FastLED.show();
        delay(150);

        LEDsectionManager.fillSectionWithColor(LED_AIRQ, CRGB::Magenta , FillStyle(ALL_AT_ONCE)); 
        FastLED.show();
        delay(500);
      }
    }
    else
    { 
      //LEDsectionManager.fillSectionWithColor(LED_AIRQ, CRGB::Black, FillStyle(ALL_AT_ONCE));
    }




    int MHZ19CO2 = myMHZ19.getCO2();
    int checkCO2 = MHZ19CO2;
    output += ", " + String(iaqSensor.co2Equivalent);
    output += ", " + String(iaqSensor.breathVocEquivalent);
    output += ", " + String(MHZ19CO2);
    if (MHZ19CO2 == 0)
    {
      checkCO2 = iaqSensor.co2Equivalent;
    }
      
    if (iaqSensor.iaqAccuracy > 0)
    {
      if (checkCO2 < 600) //outdoor air
      {  
        LEDsectionManager.fillSectionWithColor(LED_CO2, CRGB::Blue, FillStyle(ALL_AT_ONCE)); 
      }
      else if (checkCO2 < 800) // fresh indoor air
      {  
        LEDsectionManager.fillSectionWithColor(LED_CO2, CRGB::Green, FillStyle(ALL_AT_ONCE)); 
      }
      else if (checkCO2 < 1000) // Indoor air
      {  
        LEDsectionManager.fillSectionWithColor(LED_CO2, CRGB::GreenYellow, FillStyle(ALL_AT_ONCE)); 
      }
      else if (checkCO2 < 1200) // used interior air. please ventilate
      {  
         LEDsectionManager.fillSectionWithColor(LED_CO2, CRGB::Yellow, FillStyle(ALL_AT_ONCE)); 
      }
      else if (checkCO2 < 1400) //stale indoor air. please ventilate
      {  
        LEDsectionManager.fillSectionWithColor(LED_CO2, CRGB::Orange, FillStyle(ALL_AT_ONCE)); 
      }
      else if (checkCO2 < 1600) // strongly stale indoor air. please ventilate urgently. thinking performance impaired
      {  
        LEDsectionManager.fillSectionWithColor(LED_CO2, CRGB::Red, FillStyle(ALL_AT_ONCE)); 
      }
      else // Tiredness, headache. please ventilate urgently.
      {  
        LEDsectionManager.fillSectionWithColor(LED_CO2, CRGB::Black, FillStyle(ALL_AT_ONCE));
        FastLED.show();
        delay(150);

        LEDsectionManager.fillSectionWithColor(LED_CO2, CRGB::Magenta , FillStyle(ALL_AT_ONCE)); 
        FastLED.show();
        delay(500);
      }
    }


    Serial.println(output);
    FastLED.setBrightness(50);
    FastLED.show();

  }
  else
  {
    checkIaqSensorStatus();
  }
}


void errLeds(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}

void loadState(void)
{
  if (EEPROM.read(0) == BSEC_MAX_STATE_BLOB_SIZE)
  {
    // Existing state in EEPROM
    Serial.println("Reading state from EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++)
    {
      bsecState[i] = EEPROM.read(i + 1);
      Serial.println(bsecState[i], HEX);
    }

    iaqSensor.setState(bsecState);
    checkIaqSensorStatus();
  }
  else
  {
    // Erase the EEPROM with zeroes
    Serial.println("Erasing EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE + 1; i++)
      EEPROM.write(i, 0);

    EEPROM.commit();
  }
}

void clearState(void)
{
  // Erase the EEPROM with zeroes
  Serial.println("Erasing EEPROM");

  for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE + 1; i++)
    EEPROM.write(i, 0);

  EEPROM.commit();
}

void updateState(void)
{
  bool update = false;
  if (stateUpdateCounter == 0)
  {
    /* Set a trigger to save the state. Here, the state is saved every STATE_SAVE_PERIOD with the first state being saved once the algorithm achieves full calibration, i.e. iaqAccuracy = 3 */
    if (iaqSensor.iaqAccuracy >= 3)
    {
      update = true;
      stateUpdateCounter++;
    }
  }
  else
  {
    /* Update every STATE_SAVE_PERIOD milliseconds */
    if ((stateUpdateCounter * STATE_SAVE_PERIOD) < millis())
    {
      if (iaqSensor.iaqAccuracy >= 3)
      {
        update = true;
        stateUpdateCounter++;
      }
    }
  }

  if (update)
  {
    iaqSensor.getState(bsecState);
    checkIaqSensorStatus();
    //myMHZ19.calibrateZero();
    Serial.println("Writing state to EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++)
    {
      EEPROM.write(i + 1, bsecState[i]);
      Serial.println(bsecState[i], HEX);
    }

    EEPROM.write(0, BSEC_MAX_STATE_BLOB_SIZE);
    EEPROM.commit();
  }
}

