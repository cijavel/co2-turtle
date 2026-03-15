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

	Serial.println("[BME680] Initializing at fixed address 0x77 (CS hardwired HIGH)");
    bmeSensor.begin(BME68X_I2C_ADDR_HIGH, Wire);

	if (bmeSensor.bme68xStatus == BME68X_OK) {
        _sensorOk = true;
        bmeSensor.setConfig(bsec_config_iaq);
        checkSensorStatus();
        loadState();
    } else {
        _sensorOk = false;
        _bme68xError = bmeSensor.bme68xStatus;
        Serial.printf("[BME680] Init failed (bme68xStatus=%d, I2C addr=0x77)\n", _bme68xError);
    }


    #if DEBUG
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
            Serial.printf("[BME680] Recovery attempt (uptime %lus)...\n", currentSeconds);
            Wire.end();
            delay(100);
            Wire.begin(PIN_BME680_SDA, PIN_BME680_SCL);
            Wire.setClock(100000);
            delay(500);
            
            bmeSensor.begin(BME68X_I2C_ADDR_HIGH, Wire);
            delay(200);

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
                Serial.printf("[BME680] Recovery failed (bme68xStatus=%d) – retry in 60s\n", _bme68xError);
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
        Serial.printf("[BME680] Error %d (%d/5 consecutive)%s\n", _bme68xError, _consecutiveErrors, (_consecutiveErrors >= 4 ? " – recovery imminent!" : ""));
        checkSensorStatus();
        if (_consecutiveErrors >= 5)
        {
            _sensorOk = false;
            Serial.println("[BME680] Too many errors, triggering recovery.");
        }
    }
    else
    {
        _bme68xError = 0;
        _consecutiveErrors = 0; // Reset on healthy status
    }
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

Bsec BME680Handler::getData()
{
	return bmeSensor;
}

void BME680Handler::loadState(void)
{
	if (EEPROM.read(0) == BSEC_MAX_STATE_BLOB_SIZE)
	{
		Serial.printf("[BME680] Reading BSEC state from EEPROM (%d bytes)\n", BSEC_MAX_STATE_BLOB_SIZE);
    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++)
    {
        bsecState[i] = EEPROM.read(i + 1);
    }
    #if DEBUG
    Serial.printf("[BME680] EEPROM state loaded, first=0x%02X last=0x%02X\n",
        bsecState[0],
        bsecState[BSEC_MAX_STATE_BLOB_SIZE - 1]);
    #endif
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
        /* First state update when IAQ accuracy is >= 1 to preserve partial calibration */
        if (bmeSensor.iaqAccuracy >= 1)
        {
            update = true;
            stateUpdateCounter++;
            Serial.println("[BME680] First state update triggered (accuracy=" + String(bmeSensor.iaqAccuracy) + ")");
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
            Serial.printf("[BME680] Writing BSEC state to EEPROM (%d bytes), first=0x%02X last=0x%02X\n",
                BSEC_MAX_STATE_BLOB_SIZE,
                bsecState[0],
                bsecState[BSEC_MAX_STATE_BLOB_SIZE - 1]);
            for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++)
            {
                EEPROM.write(i + 1, bsecState[i]);
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