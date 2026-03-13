#include "ConfigHandler.h"
#include "Configuration.h"
#include "credentials.h"
#include <map>


static std::map<String, int> configMapforSwitch = {};
static std::map<String, int> configMapforInterval = {};
static std::map<String, String> configMapforDevice = {};
static std::map<String, int> configMapforLED = {};
static std::map<String, int> configMapforSensor = {};

void ConfigHandler::loadAllPersistedSettings()
{
	preferences.begin("config", true);
		configMapforSwitch["switchWIFI"] = preferences.getBool("switchWIFI", switch_WIFI);
		configMapforSwitch["switchEPD"] = preferences.getBool("switchEPD", switch_EPD);
		configMapforSwitch["switchEPDorientation"] = preferences.getInt("switchEPDori", switch_EPD_orientation);
		configMapforSwitch["switchLED"] = preferences.getBool("switchLED", switch_LED);
		configMapforSwitch["switchMQTT"] = preferences.getBool("switchMQTT", switch_MQTT);

		configMapforInterval["intervalMHZ19"] = preferences.getInt("intervalMHZ19", interval_MHZ19_in_Seconds);
		configMapforInterval["intervalBME680"] = preferences.getInt("intervalBME680", interval_BME680_in_Seconds);
		configMapforInterval["intervalWiFi"] = preferences.getInt("intervalWiFi", interval_WiFiCheck_in_Seconds);
		configMapforInterval["intervalPRINT"] = preferences.getInt("intervalPRINT", interval_RAMPrintout_in_Seconds);
		configMapforInterval["intervalEPD"] = preferences.getInt("intervalEPD", interval_EPD_in_Seconds);
		configMapforInterval["intervalLED"] = preferences.getInt("intervalLED", interval_LED_in_Seconds);
		configMapforInterval["intervalMQTT"] = preferences.getInt("intervalMQTT", interval_mqtt_in_Seconds);

		configMapforDevice["deviceName"] = preferences.getString("deviceName", DEVICE_NAME);
		configMapforDevice["timezone"] = preferences.getString("timezone", TIMEZONE);
		configMapforDevice["wlanSSID"] = preferences.getString("wlanSSID", WIFI_SSID);
		configMapforDevice["wlanPASSWORD"] = preferences.getString("wlanPASSWORD", WIFI_PW);
		configMapforDevice["mqttUSER"] = preferences.getString("mqttUSER", MQTT_USER);
		configMapforDevice["mqttPASSWORD"] = preferences.getString("mqttPASSWORD", MQTT_PASS);
		configMapforDevice["mqttHOST"] = preferences.getString("mqttHOST", MQTT_HOST);
		configMapforDevice["mqttPORT"] = String(preferences.getInt("mqttPORT", MQTT_PORT));
		configMapforDevice["mqttUSERen"] = preferences.getBool("mqttUSERen", MQTT_USER_ENABLED) ? "1" : "0";

		configMapforLED["LEDbrightness"] = preferences.getInt("LEDbrightness", BRIGHTNESS_LEDS);

		configMapforSensor["pressure"] = preferences.getInt("pressure", SEALEVELPRESSURE_HPA);
		configMapforSensor["tempOffset"] = preferences.getInt("tempOffset", (int)(TEMPERATUR_OFFSET * 10));
	preferences.end();
}

void ConfigHandler::validateConfigMaps()
{
    for (const auto& key : {"switchWIFI", "switchEPD", "switchEPDorientation", "switchLED", "switchMQTT"})
    {
        if (configMapforSwitch.find(key) == configMapforSwitch.end())
        {
            Serial.printf("[Config] Missing key in configMapforSwitch: %s\n", key);
        }
    }
	for (const auto& key : {"intervalBME680", "intervalWiFi", "intervalPRINT", "intervalEPD", "intervalLED", "intervalMQTT"})
	{
		if (configMapforInterval.find(key) == configMapforInterval.end())
		{
		Serial.printf("[Config] Missing key in configMapforInterval: %s\n", key);
		}
	}
    for (const auto& key : {"deviceName", "timezone", "wlanSSID", "wlanPASSWORD", "mqttUSER", "mqttPASSWORD", "mqttHOST", "mqttPORT", "mqttUSERen"})
    {
        if (configMapforDevice.find(key) == configMapforDevice.end())
        {
            Serial.printf("[Config] Missing key in configMapforDevice: %s\n", key);
        }
    }
    for (const auto& key : {"LEDbrightness"})
    {
        if (configMapforLED.find(key) == configMapforLED.end())
        {
            Serial.printf("[Config] Missing key in configMapforLED: %s\n", key);
        }
    }
    for (const auto& key : {"pressure", "tempOffset"})
    {
        if (configMapforSensor.find(key) == configMapforSensor.end())
        {
            Serial.printf("[Config] Missing key in configMapforSensor: %s\n", key);
        }
    }

    Serial.println("[Config] Validation of configMaps completed.");
}

void ConfigHandler::persistAllSettings()
{
	preferences.begin("config", false);
	preferences.putBool("switchWIFI", configMapforSwitch["switchWIFI"]);
	preferences.putBool("switchEPD", configMapforSwitch["switchEPD"]);
	preferences.putInt("switchEPDori", configMapforSwitch["switchEPDorientation"]);
	preferences.putBool("switchLED", configMapforSwitch["switchLED"]);
	preferences.putBool("switchMQTT", configMapforSwitch["switchMQTT"]);

	preferences.putInt("intervalMHZ19", configMapforInterval["intervalMHZ19"]);
	preferences.putInt("intervalBME680", configMapforInterval["intervalBME680"]);
	preferences.putInt("intervalWiFi", configMapforInterval["intervalWiFi"]);
	preferences.putInt("intervalPRINT", configMapforInterval["intervalPRINT"]);
	preferences.putInt("intervalEPD", configMapforInterval["intervalEPD"]);
	preferences.putInt("intervalLED", configMapforInterval["intervalLED"]);
	preferences.putInt("intervalMQTT", configMapforInterval["intervalMQTT"]);

	preferences.putString("deviceName",   configMapforDevice["deviceName"]);
	preferences.putString("timezone",     configMapforDevice["timezone"]);
	preferences.putString("wlanSSID",     configMapforDevice["wlanSSID"]);
	preferences.putString("wlanPASSWORD", configMapforDevice["wlanPASSWORD"]);
	preferences.putString("mqttUSER",     configMapforDevice["mqttUSER"]);
	preferences.putString("mqttPASSWORD", configMapforDevice["mqttPASSWORD"]);
	preferences.putString("mqttHOST",     configMapforDevice["mqttHOST"]);
	preferences.putInt(   "mqttPORT",     configMapforDevice["mqttPORT"].toInt());
	preferences.putBool(  "mqttUSERen",   configMapforDevice["mqttUSERen"] == "1");

	preferences.putInt("LEDbrightness", configMapforLED["LEDbrightness"]);

	preferences.putInt("pressure",   configMapforSensor["pressure"]);
	preferences.putInt("tempOffset", configMapforSensor["tempOffset"]);

	preferences.end();
}

void ConfigHandler::restoreDefaultConfiguration()
{
	Serial.println("[preferences] set configuration");
	preferences.begin("config", false);
	preferences.clear();
	preferences.putBool("setFirstRun", true);

	preferences.putBool("switchWIFI", switch_WIFI);
	preferences.putBool("switchEPD", switch_EPD);
	preferences.putInt("switchEPDori", switch_EPD_orientation);
	preferences.putBool("switchLED", switch_LED);
	preferences.putBool("switchMQTT", switch_MQTT);

	preferences.putInt("intervalMHZ19", interval_MHZ19_in_Seconds);
	preferences.putInt("intervalBME680", interval_BME680_in_Seconds);
	preferences.putInt("intervalWiFi", interval_WiFiCheck_in_Seconds);
	preferences.putInt("intervalPRINT", interval_RAMPrintout_in_Seconds);
	preferences.putInt("intervalEPD", interval_EPD_in_Seconds);
	preferences.putInt("intervalLED", interval_LED_in_Seconds);
	preferences.putInt("intervalMQTT", interval_mqtt_in_Seconds);

	preferences.putString("deviceName", DEVICE_NAME);
	preferences.putString("timezone", TIMEZONE);
	preferences.putString("wlanSSID", WIFI_SSID);
	preferences.putString("wlanPASSWORD", WIFI_PW);
	preferences.putString("mqttUSER", MQTT_USER);
	preferences.putString("mqttPASSWORD", MQTT_PASS);
	preferences.putString("mqttHOST", MQTT_HOST);
	preferences.putInt("mqttPORT", MQTT_PORT);
	preferences.putBool("mqttUSERen", MQTT_USER_ENABLED);

	preferences.putInt("pressure", SEALEVELPRESSURE_HPA);
	preferences.putInt("tempOffset", (int)(TEMPERATUR_OFFSET * 10));


	preferences.putInt("LEDbrightness", constrain(BRIGHTNESS_LEDS, 2, 255));
	preferences.end();
}

void ConfigHandler::setSettingsOnFirstRun()
{
	preferences.begin("config", true);
	bool hasRunBefore = preferences.getBool("setFirstRun", false);
	preferences.end();
	if (!hasRunBefore)
	{
		restoreDefaultConfiguration();
		Serial.println("[Config] Setting firstRun flag in NVS to true.");
	}
	else
	{
		Serial.println("[Config] FirstRun flag in NVS is already set to true.");
	}
	loadAllPersistedSettings();
	validateConfigMaps();
	//printConfigMaps();
}


/* -------------------------- Getter and Setter for Config Maps --------------------- */
void ConfigHandler::setConfigSwitch(String settingName, int value)
{
	configMapforSwitch[settingName] = value;
}
int ConfigHandler::getConfigSwitch(String settingName)
{
	return configMapforSwitch[settingName];
}

void ConfigHandler::setConfigInterval(String settingName, int value)
{
	configMapforInterval[settingName] = value;
}
int ConfigHandler::getConfigInterval(String settingName)
{
	return configMapforInterval[settingName];
}

String ConfigHandler::getConfigDevice(String settingName)
{
	return configMapforDevice[settingName];
}
void ConfigHandler::setConfigDevice(String settingName, String value)
{
	configMapforDevice[settingName] = value;
}

int ConfigHandler::getConfigLED(String settingName)
{
	return configMapforLED[settingName];
}
void ConfigHandler::setConfigLED(String settingName, int value)
{
	if (value < 2 || value > 255) {
        Serial.println("[Config] LEDbrightness value out of range. Clamping to valid range.");
        value = (value < 2) ? 2 : 255; 
    }
    configMapforLED[settingName] = value;
}

int ConfigHandler::getConfigSensor(String settingName)
{
	return configMapforSensor[settingName];
}
void ConfigHandler::setConfigSensor(String settingName, int value)
{
	configMapforSensor[settingName] = value;
}


