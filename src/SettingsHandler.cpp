#include "settingsHandler.h"
#include "Configuration.h"
#include "credentials.h"
#include <map>

static std::map<String, int> settingMap = {};

void SettingsHandler::loadsavedSettings() {
    preferences.begin("config", true);
       settingMap["switchWIFI"] = preferences.getBool("switchWIFI", switch_WIFI);
       settingMap["switchEPD"] = preferences.getBool("switchEPD", switch_EPD);
       settingMap["switchLED"] = preferences.getBool("switchLED", switch_LED);
       settingMap["switchMQTT"] = preferences.getBool("switchMQTT", switch_MQTT);

       settingMap["intervalMHZ19"] = preferences.getInt("intervalMHZ19", interval_MHZ19_in_Seconds);
       settingMap["intervalBME680"] = preferences.getInt("intervalBME680", interval_BME680_in_Seconds);
       settingMap["intervalWiFi"] = preferences.getInt("intervalWiFi", interval_WiFiCheck_in_Seconds);
       settingMap["intervalPRINT"] = preferences.getInt("intervalPRINT", interval_RAMPrintout_in_Seconds);
       settingMap["intervalEPD"] = preferences.getInt("intervalEPD", interval_EPD_in_Seconds);
       settingMap["intervalLED"] = preferences.getInt("intervalLED", interval_LED_in_Seconds);
       settingMap["intervalMQTT"] = preferences.getInt("intervalMQTT", interval_mqtt_in_Seconds);
    preferences.end();
}

int SettingsHandler::getSetting(String settingName){
 return settingMap[settingName];
}

void SettingsHandler::loadDefaultSettings() {
    Serial.println("[preferences] set configuration");
    preferences.begin("config", false);
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
        preferences.putInt("amountLED", NUM_LEDS);

        preferences.putString("wlanSSID", WIFI_SSID);
        preferences.putString("wlanPASSWORD", WIFI_PW);

        preferences.putString("mqttUSER", MQTT_USER);
        preferences.putString("mqttPASSWORD", MQTT_PASS);
        preferences.putInt("mqttHOST", MQTT_HOST);
        preferences.putInt("mqttPORT", MQTT_PORT);
        preferences.putBool("mqttUSERen", MQTT_USER_ENABLED);

        preferences.putInt("PRESSURE", SEALEVELPRESSURE_HPA);
        preferences.putInt("TEMPOFFSET", TEMPERATUR_OFFSET);
        preferences.putInt("BRIGHTNESS", BRIGHTNESS_LEDS);
    preferences.end();
}

void SettingsHandler::reset() {
    preferences.begin("config", false); 
        preferences.clear();  
    preferences.end();
}
void SettingsHandler::setSeetingsOnFirstRun() {
    preferences.begin("config", true); 
        bool hasRunBefore = preferences.getBool("setSeetingsFirstRun", false); 
    preferences.end();
    if (!hasRunBefore) {
        loadDefaultSettings();
        preferences.begin("config", false); 
            preferences.putBool("setSeetingsFirstRun", true);
        preferences.end();
        Serial.println("[Settings] Setting firstRun flag in NVS to true.");
    } 
    loadsavedSettings();
    
}