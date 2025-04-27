#include "settingsHandler.h"
#include "Configuration.h"
#include "credentials.h"

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
    preferences.begin("config", false); 
    bool hasRunBefore = preferences.getBool("setSeetingsFirstRun", false); 
    if (!hasRunBefore) {
        loadDefaultSettings();
        preferences.putBool("setSeetingsFirstRun", true);
        Serial.println("[Settings] Setting firstRun flag in NVS to true.");
    } 
    preferences.end();
}