#include "BME680Handler.h"
#include "Configuration.h"
#include "ConfigHandler.h"
extern ConfigHandler &configHandler;

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
#define STATE_SAVE_PERIOD UINT32_C(360 * 60 * 1000) // 360 minutes - 4 times a day

uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
uint16_t stateUpdateCounter = 0;

BME680Handler::BME680Handler()
{
	// IMPORTANT to set up connection with an condensator
	Wire.end();
	delay(100);
	Wire.begin(PIN_BME680_SDA, PIN_BME680_SCL);
	Wire.setClock(100000);
	delay(500);
	pinMode(LED_BUILTIN, OUTPUT);

	// IMPORTANT Retry mechanism
	uint8_t retries = 0;
    // Try address 0x76 first, then 0x77. On the board SDO is not connected and i do not want to solder to GND. So the adress could be 0x77 or 0x76
	uint8_t addresses[2] = {BME68X_I2C_ADDR_LOW, BME68X_I2C_ADDR_HIGH};
    do {
        uint8_t addr = addresses[retries % 2];
        Serial.println("[BME680] Trying address 0x" + String(addr, HEX) +
                       ", attempt " + String(retries + 1) + "/4");
        bmeSensor.begin(addr, Wire);
        if (bmeSensor.bme68xStatus == BME68X_OK)
        {
            Serial.println("[BME680] Found at address 0x" + String(addr, HEX));
            break;
        }
        Wire.end();
        delay(500);
        Wire.begin(PIN_BME680_SDA, PIN_BME680_SCL);
        Wire.setClock(100000);
        delay(500);
        retries++;
    } while (retries < 4);

	if (bmeSensor.bme68xStatus == BME68X_OK) {
        _sensorOk = true;
        bmeSensor.setConfig(bsec_config_iaq);
        checkSensorStatus();
        loadState();
    } else {
        _sensorOk = false;
        _bme68xError = bmeSensor.bme68xStatus;
        Serial.println("[BME680] Init failed after 3 retries! Error: " + String(_bme68xError));
    }


#ifdef DEBUG
	Serial.println("\n[BME] BSEC library version " + String(bmeSensor.version.major) + "." + String(bmeSensor.version.minor) + "." + String(bmeSensor.version.major_bugfix) + "." + String(bmeSensor.version.minor_bugfix));
#endif
	_sensorList[0]  = BSEC_OUTPUT_IAQ;
    _sensorList[1]  = BSEC_OUTPUT_STATIC_IAQ;
    _sensorList[2]  = BSEC_OUTPUT_CO2_EQUIVALENT;
    _sensorList[3]  = BSEC_OUTPUT_BREATH_VOC_EQUIVALENT;
    _sensorList[4]  = BSEC_OUTPUT_RAW_TEMPERATURE;
    _sensorList[5]  = BSEC_OUTPUT_RAW_PRESSURE;
    _sensorList[6]  = BSEC_OUTPUT_RAW_HUMIDITY;
    _sensorList[7]  = BSEC_OUTPUT_RAW_GAS;
    _sensorList[8]  = BSEC_OUTPUT_STABILIZATION_STATUS;
    _sensorList[9]  = BSEC_OUTPUT_RUN_IN_STATUS;
    _sensorList[10] = BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE;
    _sensorList[11] = BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY;
    _sensorList[12] = BSEC_OUTPUT_GAS_PERCENTAGE;

	if (_sensorOk)
	{
		bmeSensor.updateSubscription(_sensorList, 13, BSEC_SAMPLE_RATE_LP);
	}

	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);
} 


bool BME680Handler::updateSensorData(const unsigned long currentSeconds)
{
    // Ebene 2 – Laufzeit-Recovery wenn Sensor nicht ok
    if (!_sensorOk)
    {
        if (currentSeconds - _lastRecoverySeconds >= 60)
        {
            _lastRecoverySeconds = currentSeconds;
            Serial.println("[BME680] Attempting recovery...");
            Wire.end();
            delay(100);
            Wire.begin(PIN_BME680_SDA, PIN_BME680_SCL);
            Wire.setClock(100000);
            delay(500);
            bmeSensor.begin(BME68X_I2C_ADDR_LOW, Wire);
            if (bmeSensor.bme68xStatus == BME68X_OK)
            {
                bmeSensor.setConfig(bsec_config_iaq);
                bmeSensor.updateSubscription(_sensorList, 13, BSEC_SAMPLE_RATE_LP);
                _sensorOk = true;
                _consecutiveErrors = 0;
                _bme68xError = 0;
                Serial.println("[BME680] Recovery successful!");
                loadState();
            }
            else
            {
                _bme68xError = bmeSensor.bme68xStatus;
                Serial.println("[BME680] Recovery failed, error: " + String(_bme68xError));
            }
        }
        return false;
    }

    bool hasNewData = bmeSensor.run();

    if (hasNewData)
    {
        _consecutiveErrors = 0;
        if (currentSeconds - _lastRunSeconds >= (unsigned long)configHandler.getConfigInterval("intervalBME680"))
        {
            _lastRunSeconds = currentSeconds;
            digitalWrite(LED_BUILTIN, HIGH);
            updateState();
            digitalWrite(LED_BUILTIN, LOW);
        }
        return true;
    }

    if (bmeSensor.bme68xStatus < BME68X_OK)
    {
        _consecutiveErrors++;
        _bme68xError = bmeSensor.bme68xStatus;
        Serial.println("[BME680] Runtime error: " + String(_bme68xError) +
                       " (" + String(_consecutiveErrors) + " consecutive)");
        if (_consecutiveErrors >= 5)
        {
            _sensorOk = false;
            Serial.println("[BME680] Too many errors, triggering recovery.");
        }
    }
    checkSensorStatus();
    return false;
}

void BME680Handler::checkSensorStatus() const
{
	if (bmeSensor.bsecStatus != BSEC_OK)
	{
		if (bmeSensor.bsecStatus < BSEC_OK)
		{
			Serial.println("[BME680] BSEC error code : " + String(bmeSensor.bsecStatus));
		}
		else
		{
			Serial.println("[BME680] BSEC warning code : " + String(bmeSensor.bsecStatus));
		}
	}

	if (bmeSensor.bme68xStatus != BME68X_OK)
	{
		if (bmeSensor.bme68xStatus < BME68X_OK)
		{
			Serial.println("[BME680] BME68X error code : " + String(bmeSensor.bme68xStatus));
		}
		else
		{
			Serial.println("[BME680] BME68X warning code : " + String(bmeSensor.bme68xStatus));
		}
	}
}


void BME680Handler::printout() const
{
	Serial.println();
	Serial.println("[BME680] ");
	Serial.println("[BME680] Timestamp [ms]:               " + String(bmeSensor.outputTimestamp));
	Serial.println("[BME680] IAQ:                          " + String(bmeSensor.iaq));
	Serial.println("[BME680] IAQ accuracy:                 " + String(bmeSensor.iaqAccuracy));
	Serial.println("[BME680] IAQ Static:                   " + String(bmeSensor.staticIaq));
	Serial.println("[BME680] gas [Ohm]:                    " + String(bmeSensor.gasResistance));
	Serial.println("[BME680] pressure [hPa]:               " + String(bmeSensor.pressure / 100));
	Serial.println("[BME680] CO2 equivalent:               " + String(bmeSensor.co2Equivalent));
	Serial.println("[BME680] Stab Status:                  " + String(bmeSensor.stabStatus));
	Serial.println("[BME680] run in status:                " + String(bmeSensor.runInStatus));
	Serial.println("[BME680] gas percentage:               " + String(bmeSensor.gasPercentage));
	Serial.println("[BME680] temperature [°C]:             " + String(bmeSensor.temperature));
	Serial.println("[BME680] temperature with offset [°C]: " + String(bmeSensor.temperature + configHandler.getConfigSensor("tempOffset") / 10.0f));
	Serial.println("[BME680] raw temperature [°C]:         " + String(bmeSensor.rawTemperature));
	Serial.println("[BME680] relative humidity [%]:        " + String(bmeSensor.humidity));
	Serial.println("[BME680] raw relative humidity [%]:    " + String(bmeSensor.rawHumidity));
	Serial.println("[BME680] breath VOC equivalent [ppm]:  " + String(bmeSensor.breathVocEquivalent));
	Serial.println();
}

Bsec BME680Handler::getData()
{
	return bmeSensor;
}

void BME680Handler::loadState(void)
{
	if (EEPROM.read(0) == BSEC_MAX_STATE_BLOB_SIZE)
	{
		Serial.println("[BME680] Reading state from EEPROM");
		for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++)
		{
			bsecState[i] = EEPROM.read(i + 1);
			Serial.println(bsecState[i], HEX);
		}
		bmeSensor.setState(bsecState);
		checkSensorStatus();
	}
	else
	{
		Serial.println("[BME680] Invalid state detected, erasing EEPROM");
        for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE + 1; i++)
        {
            if (EEPROM.read(i) != 0)
            {
                EEPROM.write(i, 0);
            }
        }
		EEPROM.commit();
	}
}

void BME680Handler::updateState(void)
{
    bool update = false;
    if (stateUpdateCounter == 0)
    {
        /* First state update when IAQ accuracy is >= 3 */
        if (bmeSensor.iaqAccuracy >= 3)
        {
            update = true;
            stateUpdateCounter++;
            Serial.println("[BME680] First state update triggered");
        }
    }
    else
    {
        /* Update every STATE_SAVE_PERIOD minutes */
        if ((millis() - (stateUpdateCounter * STATE_SAVE_PERIOD)) >= STATE_SAVE_PERIOD)
        {
            update = true;
            stateUpdateCounter++;
            Serial.println("[BME680] Periodic state update triggered");
        }
    }

    if (update)
    {
        bmeSensor.getState(bsecState);
        checkSensorStatus();

        // CHeck, if the data were changed
        bool dataChanged = false;
        for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++)
        {
            if (EEPROM.read(i + 1) != bsecState[i])
            {
                dataChanged = true;
                break;
            }
        }

        if (dataChanged)
        {
            Serial.println("[BME680] Writing state to EEPROM");
            for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++)
            {
                EEPROM.write(i + 1, bsecState[i]);
                Serial.println(bsecState[i], HEX);
            }
            EEPROM.write(0, BSEC_MAX_STATE_BLOB_SIZE);
            EEPROM.commit();
        }
        else
        {
            Serial.println("[BME680] State unchanged, skipping EEPROM write");
        }
    }
}

bool BME680Handler::isSensorOk() const
{
    return _sensorOk;
}

int BME680Handler::getSensorError() const
{
    return _bme68xError;
}

bool BME680Handler::isRecovering() const
{
    return !_sensorOk && _lastRecoverySeconds > 0;
}