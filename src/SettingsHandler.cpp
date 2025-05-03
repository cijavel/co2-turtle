#include "settingsHandler.h"
#include "Configuration.h"
#include "credentials.h"
#include <map>

static std::map<String, int> configMapforModul = {};
static std::map<String, String> configMapforDevice = {};
static std::map<String, int> configMapforLED = {};
static std::map<String, int> configMapforSensor = {};

void SettingsHandler::loadAllPersistedSettings()
{
	preferences.begin("config", true);
	configMapforModul["switchWIFI"] = preferences.getBool("switchWIFI", switch_WIFI);
	configMapforModul["switchEPD"] = preferences.getBool("switchEPD", switch_EPD);
	configMapforModul["switchLED"] = preferences.getBool("switchLED", switch_LED);
	configMapforModul["switchMQTT"] = preferences.getBool("switchMQTT", switch_MQTT);

	configMapforModul["intervalMHZ19"] = preferences.getInt("intervalMHZ19", interval_MHZ19_in_Seconds);
	configMapforModul["intervalBME680"] = preferences.getInt("intervalBME680", interval_BME680_in_Seconds);
	configMapforModul["intervalWiFi"] = preferences.getInt("intervalWiFi", interval_WiFiCheck_in_Seconds);
	configMapforModul["intervalPRINT"] = preferences.getInt("intervalPRINT", interval_RAMPrintout_in_Seconds);
	configMapforModul["intervalEPD"] = preferences.getInt("intervalEPD", interval_EPD_in_Seconds);
	configMapforModul["intervalLED"] = preferences.getInt("intervalLED", interval_LED_in_Seconds);
	configMapforModul["intervalMQTT"] = preferences.getInt("intervalMQTT", interval_mqtt_in_Seconds);

	configMapforDevice["device"] = preferences.getString("device", DeviceName);
	configMapforDevice["timezone"] = preferences.getString("timezone", TIMEZONE);
	configMapforDevice["wlanSSID"] = preferences.getString("wlanSSID", WIFI_SSID);
	configMapforDevice["wlanPASSWORD"] = preferences.getString("wlanPASSWORD", WIFI_PW);
	configMapforDevice["mqttUSER"] = preferences.getString("mqttUSER", MQTT_USER);
	configMapforDevice["mqttPASSWORD"] = preferences.getString("mqttPASSWORD", MQTT_PASS);
	configMapforDevice["mqttHOST"] = preferences.getString("mqttHOST", MQTT_HOST);
	configMapforDevice["mqttPORT"] = preferences.getInt("mqttPORT", MQTT_PORT);
	configMapforDevice["mqttUSERen"] = preferences.getBool("mqttUSERen", MQTT_USER_ENABLED);

	configMapforLED["LEDbrightness"] = preferences.getInt("LEDbrightness", BRIGHTNESS_LEDS);
	configMapforLED["LEDamount"] = preferences.getInt("LEDamount", NUM_LEDS);

	configMapforSensor["pressure"] = preferences.getInt("pressure", SEALEVELPRESSURE_HPA);
	configMapforSensor["temperaturOffset"] = preferences.getInt("temperaturOffset", TEMPERATUR_OFFSET);
	preferences.end();
}

void SettingsHandler::persistAllSettings()
{
	preferences.begin("config", false);
	preferences.putBool("switchWIFI", configMapforModul["switchWIFI"]);
	preferences.putBool("switchEPD", configMapforModul["switchEPD"]);
	preferences.putBool("switchLED", configMapforModul["switchLED"]);
	preferences.putBool("switchMQTT", configMapforModul["switchMQTT"]);

	preferences.putInt("intervalMHZ19", configMapforModul["intervalMHZ19"]);
	preferences.putInt("intervalBME680", configMapforModul["intervalBME680"]);
	preferences.putInt("intervalWiFi", configMapforModul["intervalWiFi"]);
	preferences.putInt("intervalPRINT", configMapforModul["intervalPRINT"]);
	preferences.putInt("intervalEPD", configMapforModul["intervalEPD"]);
	preferences.putInt("intervalLED", configMapforModul["intervalLED"]);
	preferences.putInt("intervalMQTT", configMapforModul["intervalMQTT"]);
	preferences.end();
}

int SettingsHandler::getConfigModul(String settingName)
{
	return configMapforModul[settingName];
}

void SettingsHandler::setConfigModul(String settingName, int value)
{
	configMapforModul[settingName] = value;
}

void SettingsHandler::restoreDefaultConfiguration()
{
	Serial.println("[preferences] set configuration");
	preferences.begin("config", false);
	preferences.clear();
	preferences.putBool("setSettingsFirstRun", true);

	preferences.putBool("switchWIFI", switch_WIFI);
	preferences.putBool("switchEPD", switch_EPD);
	preferences.putBool("switchLED", switch_LED);
	preferences.putBool("switchMQTT", switch_MQTT);

	preferences.putInt("intervalMHZ19", interval_MHZ19_in_Seconds);
	preferences.putInt("intervalBME680", interval_BME680_in_Seconds);
	preferences.putInt("intervalWiFi", interval_WiFiCheck_in_Seconds);
	preferences.putInt("intervalPRINT", interval_RAMPrintout_in_Seconds);
	preferences.putInt("intervalEPD", interval_EPD_in_Seconds);
	preferences.putInt("intervalLED", interval_LED_in_Seconds);
	preferences.putInt("intervalMQTT", interval_mqtt_in_Seconds);

	preferences.putString("device", DeviceName);
	preferences.putString("timezone", TIMEZONE);
	preferences.putString("wlanSSID", WIFI_SSID);
	preferences.putString("wlanPASSWORD", WIFI_PW);
	preferences.putString("mqttUSER", MQTT_USER);
	preferences.putString("mqttPASSWORD", MQTT_PASS);
	preferences.putString("mqttHOST", MQTT_HOST);
	preferences.putInt("mqttPORT", MQTT_PORT);
	preferences.putBool("mqttUSERen", MQTT_USER_ENABLED);

	preferences.putInt("pressure", SEALEVELPRESSURE_HPA);
	preferences.putInt("temperaturOffset", TEMPERATUR_OFFSET);

	preferences.putInt("LEDbrightness", BRIGHTNESS_LEDS);
	preferences.putInt("LEDamount", NUM_LEDS);
	preferences.end();
}

void SettingsHandler::setSettingsOnFirstRun()
{
	preferences.begin("config", true);
	bool hasRunBefore = preferences.getBool("setSettingsFirstRun", false);
	preferences.end();
	if (!hasRunBefore)
	{
		restoreDefaultConfiguration();
		Serial.println("[Settings] Setting firstRun flag in NVS to true.");
	}
	loadAllPersistedSettings();
}