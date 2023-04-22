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

//Sensor
#include "bsec.h"



// ---------------------------------------------
// Configuration
// ---------------------------------------------


#define BAUDRATE 9600
String consolOUT;

// sensor BME680
#define PIN_BME680_SDA 21
#define PIN_BME680_SCL 22



// ---------------------------------------------
// Declaration
// ---------------------------------------------


// Timer
const long interval_BME680 = 15000;


unsigned long prevtimer_BME680 = 0;


unsigned long currtimer_BME680 = 0;


// --------------------------------------------------------------------------
// sensor data
// --------------------------------------------------------------------------
const String name_bme680_timestamp           = "BME680 Timestamp [ms]";
const String name_bme680_temperatur_raw      = "BME680 raw temperature [°C]";
const String name_bme680_pressure            = "BME680 pressure [hPa]";
const String name_bme680_humidity_raw        = "BME680 raw relative humidity [%]";
const String name_bme680_gas                 = "BME680 gas [Ohm]";
const String name_bme680_iaq                 = "BME680 IAQ";
const String name_bme680_iaq_accuracy        = "BME680 IAQ accuracy";
const String name_bme680_temperatur_relative = "BME680 temperature [°C]";
const String name_bme680_humidity_relative   = "BME680 relative humidity [%]";
const String name_bme680_stab                = "BME680 Stab Status"; 
const String name_bme680_iaq_static          = "BME680 IAQ Static";
const String name_bme680_co2equil            = "BME680 CO2 equivalentv";
const String name_bme680_breahtvoc           = "BME680 breath VOC equivalent [ppm]";
const String name_bme680_runinStatus         = "BME680 run in status";
const String name_bme680_percentage          = "BME680 gas percentage";
const String name_bme680_datetime            = "BME680 Date and Time";
const String name_bme680_date                = "BME680 Date";
const String name_bme680_time                = "BME680 Time";
const String name_bme680_zone                = "BME680 Timezone";

String data_bme680_timestamp           = "";
String data_bme680_iaq                 = "";
String data_bme680_iaq_accuracy        = "";
String data_bme680_iaq_static          = "";
String data_bme680_co2equil            = "";
String data_bme680_breahtvoc           = "";
String data_bme680_temperatur_raw      = "";
String data_bme680_temperatur_relative = "";
String data_bme680_humidity_raw        = "";
String data_bme680_humidity_relative   = "";
String data_bme680_pressure            = "";
String data_bme680_gas                 = "";
String data_bme680_stabStatus          = "";
String data_bme680_runInStatus         = "";
String data_bme680_percentage          = "";
String data_bme680_date                = "";
String data_bme680_time                = "";
String data_bme680_zone                = "";


// ---------------------------------------------
// Helper functions declarations
// ---------------------------------------------
void BMEcheckIaqSensorStatus(void);
void BMEerrLeds(void);


// --------------------------------------------------------------------------
// BME680 sensor
// --------------------------------------------------------------------------

// Create an object of the class Bsec
Bsec iaqSensor;
//save calibration data
#define STATE_SAVE_PERIOD UINT32_C(1440 * 60 * 1000) // 1440 minutes - 1 times a day
uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
uint16_t stateUpdateCounter = 0;

void BMEcheckIaqSensorStatus(void){
  if (iaqSensor.bsecStatus != BSEC_OK) {
    if (iaqSensor.bsecStatus < BSEC_OK) {
      consolOUT = "BSEC error code : " + String(iaqSensor.bsecStatus);
      Serial.println(consolOUT);
      for (;;)
        BMEerrLeds(); /* Halt in case of failure */
    } else {
      consolOUT = "BSEC warning code : " + String(iaqSensor.bsecStatus);
      Serial.println(consolOUT);
    }
  }

  if (iaqSensor.bme68xStatus != BME68X_OK) {
    if (iaqSensor.bme68xStatus < BME68X_OK) {
      consolOUT = "BME68X error code : " + String(iaqSensor.bme68xStatus);
      Serial.println(consolOUT);
      for (;;)
        BMEerrLeds(); /* Halt in case of failure */
    } else {
      consolOUT = "BME68X warning code : " + String(iaqSensor.bme68xStatus);
      Serial.println(consolOUT);
    }
  }
}

void BMEerrLeds(void){
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}

void BMEsetup(){
  // WICHTIG
  Wire.begin(PIN_BME680_SDA, PIN_BME680_SCL);
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);

  // WICHTIG
  iaqSensor.begin(BME68X_I2C_ADDR_HIGH, Wire);
  consolOUT = "\nBSEC library version " + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
  Serial.println(consolOUT);
  BMEcheckIaqSensorStatus();

  bsec_virtual_sensor_t sensorList[13] = {
    BSEC_OUTPUT_IAQ,
    BSEC_OUTPUT_STATIC_IAQ,
    BSEC_OUTPUT_CO2_EQUIVALENT,
    BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    BSEC_OUTPUT_RAW_TEMPERATURE,
    BSEC_OUTPUT_RAW_PRESSURE,
    BSEC_OUTPUT_RAW_HUMIDITY,
    BSEC_OUTPUT_RAW_GAS,
    BSEC_OUTPUT_STABILIZATION_STATUS,
    BSEC_OUTPUT_RUN_IN_STATUS,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
    BSEC_OUTPUT_GAS_PERCENTAGE
  };

  iaqSensor.updateSubscription(sensorList, 13, BSEC_SAMPLE_RATE_LP);
  BMEcheckIaqSensorStatus();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void BMEsetValues(){
  if (iaqSensor.run()) { // If new data is available
    digitalWrite(LED_BUILTIN, HIGH);
    data_bme680_timestamp           = String(millis());
    data_bme680_iaq                 = String(iaqSensor.iaq);
    data_bme680_iaq_accuracy        = String(iaqSensor.iaqAccuracy);
    data_bme680_iaq_static          = String(iaqSensor.staticIaq);
    data_bme680_co2equil            = String(iaqSensor.co2Equivalent);
    data_bme680_breahtvoc           = String(iaqSensor.breathVocEquivalent);
    data_bme680_temperatur_raw      = String(iaqSensor.rawTemperature);
    data_bme680_temperatur_relative = String(iaqSensor.temperature);
    data_bme680_humidity_raw        = String(iaqSensor.rawHumidity);
    data_bme680_humidity_relative   = String(iaqSensor.humidity);
    data_bme680_pressure            = String(iaqSensor.pressure);
    data_bme680_gas                 = String(iaqSensor.gasResistance);
    data_bme680_stabStatus          = String(iaqSensor.stabStatus);
    data_bme680_runInStatus         = String(iaqSensor.runInStatus);
    data_bme680_percentage          = String(iaqSensor.gasPercentage);
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    BMEcheckIaqSensorStatus();
  }
}

void BMEprintout(){
  consolOUT = "BME680: \n";
  consolOUT += name_bme680_timestamp            + ":     "            + data_bme680_timestamp          + ", \n";
  consolOUT += name_bme680_iaq                  + ":                " + data_bme680_iaq                + ", \n"; 
  consolOUT += name_bme680_iaq_accuracy         + ":       "          + data_bme680_iaq_accuracy       + ", \n"; 
  consolOUT += name_bme680_iaq_static           + ":         "        + data_bme680_iaq_static         + ", \n"; 
  consolOUT += name_bme680_gas                  + ":          "       + data_bme680_gas                + ", \n";   
  consolOUT += name_bme680_pressure             + ":     "            + data_bme680_pressure           + ", \n";
  consolOUT += name_bme680_co2equil             + ":    "             + data_bme680_co2equil           + ", \n"; 
  consolOUT += name_bme680_stab                 + ":        "         + data_bme680_stabStatus         + ", \n";
  consolOUT += name_bme680_runinStatus          + ":      "           + data_bme680_runInStatus        + ", \n";
  consolOUT += name_bme680_percentage           + ":     "            + data_bme680_percentage         + ", \n";
  consolOUT += name_bme680_temperatur_relative  + ":           "      + data_bme680_temperatur_relative+ ", \n";
  consolOUT += name_bme680_temperatur_raw       + ":       "          + data_bme680_temperatur_raw     + ", \n"; 
  consolOUT += name_bme680_humidity_relative    + ":      "           + data_bme680_humidity_relative  + ", \n";
  consolOUT += name_bme680_humidity_raw         + ":  "               + data_bme680_humidity_raw       + ", \n";
  consolOUT += name_bme680_breahtvoc            + ": "                + data_bme680_breahtvoc          + ", \n"; 

  Serial.println(consolOUT);
  consolOUT = "";
}


// --------------------------------------------------------------------------
// MAIN
// --------------------------------------------------------------------------
void setup()
{
  delay(100);
  Serial.begin(BAUDRATE);
  Serial.println();

  BMEsetup();
}


void loop() {


  // BME680
  currtimer_BME680 = millis();
  if (currtimer_BME680 - prevtimer_BME680 >= interval_BME680){
    BMEsetValues();
    BMEprintout();
    prevtimer_BME680 = currtimer_BME680;
  }



}