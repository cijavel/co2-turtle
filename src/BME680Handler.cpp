#include "BME680Handler.h"
#include "Configuration.h"

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



/* Configure the BSEC library with information about the sensor
    18v/33v = Voltage at Vdd. 1.8V or 3.3V
    3s/300s = BSEC operating mode, BSEC_SAMPLE_RATE_LP or BSEC_SAMPLE_RATE_ULP
    4d/28d = Operating age of the sensor in days
    generic_18v_3s_4d
    generic_18v_3s_28d
    generic_18v_300s_4d
    generic_18v_300s_28d
    generic_33v_3s_4d
    generic_33v_3s_28d
    generic_33v_300s_4d
    generic_33v_300s_28d
*/
    const uint8_t bsec_config_iaq[] = {
    #include "config/generic_33v_3s_4d/bsec_iaq.txt"
    };
    #define STATE_SAVE_PERIOD	UINT32_C(360 * 60 * 1000) // 360 minutes - 4 times a day

    uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
    uint16_t stateUpdateCounter = 0;
    


bool BME680Handler::updateSensorData(const unsigned long currentSeconds) {
    if (currentSeconds % interval_BME680_in_Seconds == 0){
        updateSensorDataInternal();
        return true;
    }
    updateState();
    return false;
}

void BME680Handler::updateSensorDataInternal(){
    digitalWrite(LED_BUILTIN, HIGH);
    bool hasNewData = data.run();
    digitalWrite(LED_BUILTIN, LOW);
    if (!hasNewData) { // If new data is available
        checkSensorStatus();
    }
}

void BME680Handler::executeLedError(){
    while(true) {
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
    }
}

void BME680Handler::checkSensorStatus() const{
    if (data.bsecStatus != BSEC_OK) {
        if (data.bsecStatus < BSEC_OK) {
            Serial.println("BME: BSEC error code : " + String(data.bsecStatus));
            executeLedError(); /* Halt in case of failure */
        } else {
            Serial.println("BME: BSEC warning code : " + String(data.bsecStatus));
        }
    }

    if (data.bme68xStatus != BME68X_OK) {
        if (data.bme68xStatus < BME68X_OK) {
            Serial.println("BME: BME68X error code : " + String(data.bme68xStatus));

            executeLedError(); /* Halt in case of failure */
        } else {
            Serial.println("BME: BME68X warning code : " + String(data.bme68xStatus));
        }
    }
}

// setup
BME680Handler::BME680Handler(){
    // WICHTIG
    Wire.begin(PIN_BME680_SDA, PIN_BME680_SCL);
    delay(1000);
    pinMode(LED_BUILTIN, OUTPUT);

    // WICHTIG
    data.begin(BME68X_I2C_ADDR_HIGH, Wire);
#ifdef DEBUG
    Serial.println("\nBME: BSEC library version " + String(data.version.major) + "." + String(data.version.minor) + "." + String(data.version.major_bugfix) + "." + String(data.version.minor_bugfix));
#endif
    data.setConfig(bsec_config_iaq);
    checkSensorStatus();

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

    data.updateSubscription(sensorList, 13, BSEC_SAMPLE_RATE_LP);

    
    checkSensorStatus();
    loadState();

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
}

void BME680Handler::printout() const {
    Serial.println("BME680:");
    Serial.println();
    Serial.println(name_bme680_timestamp            + ":     "            + String(data.outputTimestamp)    );
    Serial.println(name_bme680_iaq                  + ":                " + String(data.iaq)                );
    Serial.println(name_bme680_iaq_accuracy         + ":       "          + String(data.iaqAccuracy)        );
    Serial.println(name_bme680_iaq_static           + ":         "        + String(data.staticIaq)          );
    Serial.println(name_bme680_gas                  + ":          "       + String(data.gasResistance)      );
    Serial.println(name_bme680_pressure             + ":     "            + String(data.pressure)           );
    Serial.println(name_bme680_co2equil             + ":    "             + String(data.co2Equivalent)      );
    Serial.println(name_bme680_stab                 + ":        "         + String(data.stabStatus)         );
    Serial.println(name_bme680_runinStatus          + ":      "           + String(data.runInStatus)        );
    Serial.println(name_bme680_percentage           + ":     "            + String(data.gasPercentage)      );
    Serial.println(name_bme680_temperatur_relative  + ":           "      + String(data.temperature)        );
    Serial.println(name_bme680_temperatur_raw       + ":       "          + String(data.rawTemperature)     );
    Serial.println(name_bme680_humidity_relative    + ":      "           + String(data.humidity)           );
    Serial.println(name_bme680_humidity_raw         + ":  "               + String(data.rawHumidity)        );
    Serial.println(name_bme680_breahtvoc            + ": "                + String(data.breathVocEquivalent));
    Serial.println();
}

Bsec BME680Handler::getData() {
    return data;
}

void BME680Handler::loadState(void)
{
  if (EEPROM.read(0) == BSEC_MAX_STATE_BLOB_SIZE) {
    // Existing state in EEPROM
    Serial.println("Reading state from EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++) {
      bsecState[i] = EEPROM.read(i + 1);
      Serial.println(bsecState[i], HEX);
    }

    data.setState(bsecState);

    checkSensorStatus();

  } else {
    // Erase the EEPROM with zeroes
    Serial.println("Erasing EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE + 1; i++)
      EEPROM.write(i, 0);

    EEPROM.commit();
  }
}

void BME680Handler::updateState(void)
{
  bool update = false;
  if (stateUpdateCounter == 0) {
    /* First state update when IAQ accuracy is >= 3 */
    if (data.iaqAccuracy >= 3) {
      update = true;
      stateUpdateCounter++;
    }
  } else {
    /* Update every STATE_SAVE_PERIOD minutes */
    if ((stateUpdateCounter * STATE_SAVE_PERIOD) < millis()) {
      update = true;
      stateUpdateCounter++;
    }
  }

  if (update) {
    data.getState(bsecState);
    checkSensorStatus();

    Serial.println("Writing state to EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE ; i++) {
      EEPROM.write(i + 1, bsecState[i]);
      Serial.println(bsecState[i], HEX);
    }

    EEPROM.write(0, BSEC_MAX_STATE_BLOB_SIZE);
    EEPROM.commit();
  }
}
