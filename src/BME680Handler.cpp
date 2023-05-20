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

bool BME680Handler::updateSensorData(const unsigned long currentSeconds) {
    if (currentSeconds % interval_BME680_in_Seconds == 0){
        updateSensorDataInternal();
        return true;
    }
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
