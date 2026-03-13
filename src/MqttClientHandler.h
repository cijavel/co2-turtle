#ifndef CO2_TURTLE_MQTTCLIENTHANDLER_H
#define CO2_TURTLE_MQTTCLIENTHANDLER_H

#include <AsyncMqttClient.h>
#include "WiFiHandler.h"
#include <WiFi.h>

#include "DataCO2.h"
#include "bsec.h"

class MqttClientHandler
{
public:
    static MqttClientHandler &getInstance()
    {
        static MqttClientHandler instance;
        return instance;
    }
    void setup_Mqtt();
    bool isSetup() const { return _isSetup; }
    static bool isConnected();
    static void publishData(const DataCO2 co2, const Bsec bme_data, unsigned long currentSeconds);
    static void publishDiscovery();

private:
    static void WiFiEvent(WiFiEvent_t event);
    static void connectToMqtt();
    static void onMqttConnect(bool sessionPresent);
    static void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
    bool _isSetup = false;
    String _mqttHost;   // lifetime anchor – AsyncMqttClient holds a const char* pointer, not a copy
    String _mqttUser;   // lifetime anchor – same reason
    String _mqttPass;   // lifetime anchor – same reason
    static String _deviceId;        // deviceName aus NVS, normalisiert (lowercase, spaces→-)
    static String _topicBase;       // "homeassistant/sensor/<deviceId>"
    static unsigned long _lastRunSeconds;
};
#endif // CO2_TURTLE_MQTTCLIENTHANDLER_H
