#ifndef CO2_TURTLE_MQTTCLIENTHANDLER_H
#define CO2_TURTLE_MQTTCLIENTHANDLER_H

#include <AsyncMqttClient.h>
#include "WiFiHandler.h"
#include <WiFi.h>

#include "DataCO2.h"
#include "bsec.h"


class MqttClientHandler{
public:
    static MqttClientHandler &getInstance() {
        static MqttClientHandler instance; // Guaranteed to be destroyed.
        return instance;// Instantiated on first use.

    }
    static void publishData(const DataCO2 co2, const Bsec bme_data, unsigned long currentSeconds);
    void setup_Mqtt();

private:
    static void WiFiEvent(WiFiEvent_t event);
    static void connectToMqtt();
    static void disconnectfromMqtt();
    static void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);

};
#endif //CO2_TURTLE_MQTTCLIENTHANDLER_H
