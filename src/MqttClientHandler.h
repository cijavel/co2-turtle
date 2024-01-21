#ifndef CO2_TURTLE_MQTTCLIENTHANDLER_H
#define CO2_TURTLE_MQTTCLIENTHANDLER_H

#include <AsyncMqttClient.h>
#include "WiFiHandler.h"
#include <WiFi.h>

#include "CO2Data.h"
#include "bsec.h"


class MqttClientHandler{
public:
    static MqttClientHandler &getInstance() {
        static MqttClientHandler instance; // Guaranteed to be destroyed.
        return instance;// Instantiated on first use.

    }
    void publishData(const CO2Data co2, const Bsec bme_data, const unsigned long currentSeconds);
    void setup_Mqtt();
private:
    static void WiFiEvent(WiFiEvent_t event);
    static void connectToMqtt();
    static void onMqttConnect(bool sessionPresent);
    static void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
    static void onMqttPublish(uint16_t packetId);
    


};
#endif //CO2_TURTLE_MQTTCLIENTHANDLER_H
