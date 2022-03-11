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


const uint8_t bsec_config_iaq[] = {
//#include "config/generic_33v_300s_28d/bsec_iaq.txt"
#include "config/generic_33v_300s_4d/bsec_iaq.txt"
};

// Helper functions declarations
void checkIaqSensorStatus(void);
void errLeds(void);
void loadState(void);
void updateState(void);
void clearState(void);

#define STATE_SAVE_PERIOD UINT32_C(360 * 60 * 1000) // 360 minutes - 4 times a day


// --------------------------------------------------------------------------
// Create an object of the class Bsec
Bsec iaqSensor;
uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
uint16_t stateUpdateCounter = 0;
uint32_t millisOverflowCounter = 0;
uint32_t lastTime = 0;
String output = "";
const String header = "Timestamp [ms], raw temperature [°C], pressure [hPa], raw relative humidity [%], gas [Ohm], IAQ, IAQ accuracy, temperature [°C], relative humidity [%], Static IAQ, CO2 equivalent, breath VOC equivalent, MH Z-19B CO2";
int latest_accuracy = 0;

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
//WLAN
const char *ssid     = "";
const char *password = "";

void WiFiReStart( const char *input_ssid, const char *input_password)
{
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(input_ssid);

  WiFi.begin(input_ssid, input_password);
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


// --------------------------------------------------------------------------
// definition of LED

#define NUM_LEDS 34
#define DATA_PIN 5

#define LED_WLANCONNECT 0
#define LED_STATUS 3
#define LED_TEMP 8
#define LED_HUM 15
#define LED_AIRQ 22
#define LED_CO2 29

CRGB led[NUM_LEDS];
SectionManager LEDsectionManager = SectionManager(led);

void addLEDsection()
{
  LEDsectionManager.addSections(6);
  LEDsectionManager.addRangeToSection(0, 0, 0, false);   // WLAN
  LEDsectionManager.addRangeToSection(1, 2, 5, false);   // LED_STATUS
  LEDsectionManager.addRangeToSection(2, 7, 12, false);  // LED_TEMP
  LEDsectionManager.addRangeToSection(3, 14, 19, false); // LED_HUM
  LEDsectionManager.addRangeToSection(4, 21, 26, false); // LED_AIRQ
  LEDsectionManager.addRangeToSection(5, 28, 33, false); // LED_CO2
}

void rainbowAllSections(uint8_t pauseDuration, int repeat)
{
  int colorsteps = 240; // how many colors, 256 all color, 
  int colors = 9 ;  // circle Abstand zwischen den Farben pro takt. je höher desto feiner
  int colorrun =  NUM_LEDS + repeat;
  uint16_t level, wheelPosition;

  for (wheelPosition = 0; wheelPosition < colorrun ; wheelPosition++) 
  {
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
  }
}




// --------------------------------------------------------------------------
// Server

AsyncWebServer server(80);

#define RX_PIN 18
#define TX_PIN 19
#define BAUDRATE 9600

MHZ19 myMHZ19;
HardwareSerial mySerial(1);

void handle_NotFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain; charset=utf-8", "Not found");
}

void handle_data(AsyncWebServerRequest *request)
{
  request->send(200, "text/plain; charset=utf-8", header + "\n" + output);
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




// Entry point for the example
void setup(void)
{

  // FastLED.addLeds<WS2812B, DATA_PIN, GRB>(led, NUM_LEDS);

  Serial.begin(9600);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  int wifiWaitCount = 0;

  addLEDsection();
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(led, NUM_LEDS);
  FastLED.clear(true);

  while (WiFi.status() != WL_CONNECTED && wifiWaitCount < 20)
  {
    delay(500);
    Serial.print(".");
    wifiWaitCount++;
    FastLED.setBrightness(wifiWaitCount * 2);
    LEDsectionManager.fillSectionWithColor(0, CRGB::DarkBlue, FillStyle(ALL_AT_ONCE));
    FastLED.show();
  }
  // Print local IP address and start web server
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
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

  // Print the header
  Serial.println(header);
  Serial.println("---------- Setup finished -------------");
}

// Function that is looped forever
void loop(void)
{
  CRGB oldStatus = led[LED_WLANCONNECT]; // let the LED blink if WLAN connection is gone
  if (WiFi.status() != WL_CONNECTED)
  {
    
    LEDsectionManager.fillSectionWithColor(0, CRGB::Green, FillStyle(ALL_AT_ONCE)); // LED_WLANCONNECT
    FastLED.show();
    delay(150);

    LEDsectionManager.fillSectionWithColor(0, oldStatus, FillStyle(ALL_AT_ONCE)); // LED_WLANCONNECT
    FastLED.show();
    WiFiReStart(ssid, password);
    delay(5000);
  }
  else
  {
    LEDsectionManager.fillSectionWithColor(0, CRGB::SeaGreen, FillStyle(ALL_AT_ONCE)); // LED_WLANCONNECT
    FastLED.show();
    delay(150);

    LEDsectionManager.fillSectionWithColor(0, oldStatus, FillStyle(ALL_AT_ONCE)); // LED_WLANCONNECT
    FastLED.show();
    delay(10000);
  }

  unsigned long time_trigger = millis();
  if (iaqSensor.run())
  { // If new data is available
    output = String(time_trigger);
    output += ", " + String(iaqSensor.rawTemperature);
    output += ", " + String(iaqSensor.pressure);
    output += ", " + String(iaqSensor.rawHumidity);
    output += ", " + String(iaqSensor.gasResistance);
    output += ", " + String(iaqSensor.iaq);
    output += ", " + String(iaqSensor.iaqAccuracy);


    if (iaqSensor.iaqAccuracy == 0)
    {
      rainbowAllSections(20, 100);
    }
    else if (iaqSensor.iaqAccuracy == 1)
    {
      LEDsectionManager.fillSectionWithColor(1, CRGB::Yellow, FillStyle(ALL_AT_ONCE)); // LED_STATUS
    }
    else if (iaqSensor.iaqAccuracy == 2)
    {
      LEDsectionManager.fillSectionWithColor(1, CRGB::Green, FillStyle(ALL_AT_ONCE)); // LED_STATUS
    }
    else if (iaqSensor.iaqAccuracy >= 3)
    {
      LEDsectionManager.fillSectionWithColor(1, CRGB::Green, FillStyle(ALL_AT_ONCE)); // LED_STATUS
      updateState();
    }





    output += ", " + String(iaqSensor.temperature);
    if (iaqSensor.iaqAccuracy > 0)
    {
      if (iaqSensor.temperature < 19)
      {
        LEDsectionManager.fillSectionWithColor(2, CRGB::Magenta, FillStyle(ALL_AT_ONCE)); // LED_TEMP
      }
      else if (iaqSensor.temperature < 20)
      {
        LEDsectionManager.fillSectionWithColor(2, CRGB::SeaGreen, FillStyle(ALL_AT_ONCE)); // LED_TEMP
      }
      else if (iaqSensor.temperature > 26)
      {
        LEDsectionManager.fillSectionWithColor(2, CRGB::Orange, FillStyle(ALL_AT_ONCE)); // LED_TEMP
      }
      else if (iaqSensor.temperature > 28)
      {
        LEDsectionManager.fillSectionWithColor(2, CRGB::Red, FillStyle(ALL_AT_ONCE)); // LED_TEMP
      }
      else
      {
        LEDsectionManager.fillSectionWithColor(2, CRGB::Green, FillStyle(ALL_AT_ONCE)); // LED_TEMP
      }
    }
    else
    {
      //LEDsectionManager.fillSectionWithColor(2, CRGB::Black, FillStyle(ALL_AT_ONCE)); // LED_TEMP
    }
    




    output += ", " + String(iaqSensor.humidity);
    if (iaqSensor.iaqAccuracy > 0)
    {
      if (iaqSensor.humidity < 20)
      {
        LEDsectionManager.fillSectionWithColor(3, CRGB::Red, FillStyle(ALL_AT_ONCE)); // LED_HUM
      }
      else if (iaqSensor.humidity < 30)
      {
        LEDsectionManager.fillSectionWithColor(3, CRGB::Orange, FillStyle(ALL_AT_ONCE)); // LED_HUM
      }
      else if (iaqSensor.humidity > 60)
      {
        LEDsectionManager.fillSectionWithColor(3, CRGB::SeaGreen, FillStyle(ALL_AT_ONCE)); // LED_HUM
      }
      else if (iaqSensor.humidity > 70)
      {
        LEDsectionManager.fillSectionWithColor(3, CRGB::Magenta, FillStyle(ALL_AT_ONCE)); // LED_HUM
      }
      else
      {
        LEDsectionManager.fillSectionWithColor(3, CRGB::Green, FillStyle(ALL_AT_ONCE)); // LED_HUM
      }
    }
    else
    {
         //LEDsectionManager.fillSectionWithColor(3, CRGB::Black, FillStyle(ALL_AT_ONCE)); // LED_HUM
    }




    output += ", " + String(iaqSensor.staticIaq);
    if (iaqSensor.iaqAccuracy > 0)
    {
      if (iaqSensor.iaq <= 50)
      {  
        LEDsectionManager.fillSectionWithColor(4, CRGB::Green, FillStyle(ALL_AT_ONCE)); // LED_AIRQ
      }
      else if (iaqSensor.iaq <= 100)
      {  
        LEDsectionManager.fillSectionWithColor(4, CRGB::YellowGreen, FillStyle(ALL_AT_ONCE)); // LED_AIRQ
      }
      else if (iaqSensor.iaq <= 150)
      {  
        LEDsectionManager.fillSectionWithColor(4, CRGB::Yellow, FillStyle(ALL_AT_ONCE)); // LED_AIRQ
      }
      else if (iaqSensor.iaq <= 200)
      {  
        LEDsectionManager.fillSectionWithColor(4, CRGB::Orange, FillStyle(ALL_AT_ONCE)); // LED_AIRQ
      }
      else if (iaqSensor.iaq < 250)
      {  
        LEDsectionManager.fillSectionWithColor(4, CRGB::Red, FillStyle(ALL_AT_ONCE)); // LED_AIRQ
      }
      else
      {  
        LEDsectionManager.fillSectionWithColor(4, CRGB::Magenta, FillStyle(ALL_AT_ONCE)); // LED_AIRQ
      }
    }
    else
    { 
      //LEDsectionManager.fillSectionWithColor(4, CRGB::Black, FillStyle(ALL_AT_ONCE)); // LED_AIRQ
    }



    output += ", " + String(iaqSensor.co2Equivalent);
    int MHZ19CO2 = myMHZ19.getCO2();
    int checkCO2 = MHZ19CO2;
    if (MHZ19CO2 == 0)
      checkCO2 = iaqSensor.co2Equivalent;

    if (iaqSensor.iaqAccuracy > 0)
    {
      if (checkCO2 < 600)
      {  
        LEDsectionManager.fillSectionWithColor(5, CRGB::Green, FillStyle(ALL_AT_ONCE)); // LED_CO2
      }
      else if (checkCO2 < 800)
      {  
        LEDsectionManager.fillSectionWithColor(5, CRGB::YellowGreen, FillStyle(ALL_AT_ONCE)); // LED_CO2
      }
      else if (checkCO2 < 900)
      {  
         LEDsectionManager.fillSectionWithColor(5, CRGB::Yellow, FillStyle(ALL_AT_ONCE)); // LED_CO2
      }
      else if (checkCO2 < 1000)
      {  
        LEDsectionManager.fillSectionWithColor(5, CRGB::Orange, FillStyle(ALL_AT_ONCE)); // LED_CO2
      }
      else if (checkCO2 < 1200)
      {  
        LEDsectionManager.fillSectionWithColor(5, CRGB::Red, FillStyle(ALL_AT_ONCE)); // LED_CO2
      }
      else
      {  
        LEDsectionManager.fillSectionWithColor(5, CRGB::Magenta, FillStyle(ALL_AT_ONCE)); // LED_CO2
      }
    }
    else
    { 
      //LEDsectionManager.fillSectionWithColor(5, CRGB::Black, FillStyle(ALL_AT_ONCE)); // LED_CO2
    }

    output += ", " + String(iaqSensor.breathVocEquivalent);
    output += ", " + String(MHZ19CO2);

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

//https://github.com/cyberman54/ESP32-Paxcounter/blob/master/src/bmesensor.cpp
//https://www.rehva.eu/fileadmin/user_upload/REHVA_COVID-19_guidance_document_V3_03082020.pdf
//https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME680-DS001.pdf
