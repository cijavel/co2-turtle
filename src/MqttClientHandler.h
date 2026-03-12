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
    String _mqttHost;
    String _mqttUser;
    String _mqttPass;
    static unsigned long _lastRunSeconds;
};
#endif // CO2_TURTLE_MQTTCLIENTHANDLER_H
