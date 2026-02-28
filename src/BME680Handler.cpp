#include "BME680Handler.h"
#include "Configuration.h"
#include "ConfigHandler.h"
extern ConfigHandler configHandler;

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
	// IMPORTANT
	Wire.begin(PIN_BME680_SDA, PIN_BME680_SCL);
	delay(1000);
	pinMode(LED_BUILTIN, OUTPUT);

	// IMPORTANT
	bmeSensor.begin(BME68X_I2C_ADDR_HIGH, Wire);
	bmeSensor.setConfig(bsec_config_iaq);
	checkSensorStatus();
	loadState();


#ifdef DEBUG
	Serial.println("\n[BME] BSEC library version " + String(bmeSensor.version.major) + "." + String(bmeSensor.version.minor) + "." + String(bmeSensor.version.major_bugfix) + "." + String(bmeSensor.version.minor_bugfix));
#endif
	bsec_virtual_sensor_t sensorList[13] = {
		BSEC_OUTPUT_IAQ,                                // Indoor Air Quality
		BSEC_OUTPUT_STATIC_IAQ,                        // Static IAQ
		BSEC_OUTPUT_CO2_EQUIVALENT,                    // CO2 equivalent
		BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,             // Breath VOC equivalent
		BSEC_OUTPUT_RAW_TEMPERATURE,                   // Raw temperature
		BSEC_OUTPUT_RAW_PRESSURE,                      // Raw pressure
		BSEC_OUTPUT_RAW_HUMIDITY,                      // Raw humidity
		BSEC_OUTPUT_RAW_GAS,                           // Raw gas resistance
		BSEC_OUTPUT_STABILIZATION_STATUS,              // Stabilization status
		BSEC_OUTPUT_RUN_IN_STATUS,                     // Run-in status
		BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE, // Compensated temperature
		BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,  // Compensated humidity
		BSEC_OUTPUT_GAS_PERCENTAGE                     // Gas percentage
	};

	bmeSensor.updateSubscription(sensorList, 13, BSEC_SAMPLE_RATE_LP);

	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);
} 


bool BME680Handler::updateSensorData(const unsigned long currentSeconds)
{

	if (currentSeconds % configHandler.getConfigInterval("intervalBME680") == 0)
	{
		updateState();
		updateSensorDataInternal();
		return true;
	}
	return false;
}

void BME680Handler::updateSensorDataInternal()
{
	digitalWrite(LED_BUILTIN, HIGH);
	bool hasNewData = bmeSensor.run();
	digitalWrite(LED_BUILTIN, LOW);
	if (!hasNewData)
	{ 
		checkSensorStatus();
	}
}

void BME680Handler::executeLedError()
{
	while (true)
	{
		pinMode(LED_BUILTIN, OUTPUT);
		digitalWrite(LED_BUILTIN, HIGH);
		delay(500);
		digitalWrite(LED_BUILTIN, LOW);
		delay(500);
	}
}

void BME680Handler::checkSensorStatus() const
{
	if (bmeSensor.bsecStatus != BSEC_OK)
	{
		if (bmeSensor.bsecStatus < BSEC_OK)
		{
			Serial.println("[BME680] BSEC error code : " + String(bmeSensor.bsecStatus));
			executeLedError(); 
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

			executeLedError(); 
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


