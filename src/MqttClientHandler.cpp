#include "MqttClientHandler.h"
#include "Configuration.h"
#include "Credentials.h"
#include <WiFi.h>



extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>

AsyncMqttClient mqttClient;

void MqttClientHandler::connectToMqtt() {
  if (mqttClient.connected()==false){
      mqttClient.connect();
  } 
}

void MqttClientHandler::disconnectfromMqtt() {
      if (mqttClient.connected()==true){
        mqttClient.disconnect();
      } 
}
       


void MqttClientHandler::WiFiEvent(WiFiEvent_t event) {
    Serial.printf("[MQTT] event: %d\n", event);
    wl_status_t status = WiFiClass::status();
    if (status != WL_CONNECTED)
    {
        Serial.println("[MQTT] lost connection");
    }
    else {
        Serial.println("[MQTT] reconnection");
        connectToMqtt();
    }
}


void MqttClientHandler::onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("[MQTT] Disconnected");
}


void MqttClientHandler::setup_Mqtt() {
  WiFi.onEvent(WiFiEvent);

  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCredentials(MQTT_USER, MQTT_PASS);
  Serial.println("[MQTT] connected");
}



void MqttClientHandler::publishData(const CO2Data data_co2, const Bsec data_bme, const unsigned long currentSeconds){
    if (currentSeconds % interval_mqtt_in_Seconds == 0){

        connectToMqtt();


        if (mqttClient.connected()==false){
          mqttClient.publish("sensor/bme680/temperature"           , 1, true, String(data_bme.temperature).c_str());
          mqttClient.publish("sensor/bme680/temperature_raw"       , 1, true, String(data_bme.rawTemperature).c_str());
          mqttClient.publish("sensor/bme680/humidity"              , 1, true, String(data_bme.humidity).c_str());
          mqttClient.publish("sensor/bme680/humidity_raw"          , 1, true, String(data_bme.rawHumidity).c_str());
          mqttClient.publish("sensor/bme680/pressure"              , 1, true, String(data_bme.pressure).c_str());
          mqttClient.publish("sensor/bme680/gas"                   , 1, true, String(data_bme.gasResistance).c_str());
          mqttClient.publish("sensor/bme680/bme68xStatus"          , 1, true, String(data_bme.bme68xStatus).c_str());
          mqttClient.publish("sensor/bme680/breathVocAccuracy"     , 1, true, String(data_bme.breathVocAccuracy).c_str());
          mqttClient.publish("sensor/bme680/breathVocEquivalent"   , 1, true, String(data_bme.breathVocEquivalent).c_str());
          mqttClient.publish("sensor/bme680/bsecStatus"            , 1, true, String(data_bme.bsecStatus).c_str());
          mqttClient.publish("sensor/bme680/co2Accuracy"           , 1, true, String(data_bme.co2Accuracy).c_str());
          mqttClient.publish("sensor/bme680/co2Equivalent"         , 1, true, String(data_bme.co2Equivalent).c_str());
          mqttClient.publish("sensor/bme680/compGasAccuracy"       , 1, true, String(data_bme.compGasAccuracy).c_str());
          mqttClient.publish("sensor/bme680/compGasValue"          , 1, true, String(data_bme.compGasValue).c_str());
          mqttClient.publish("sensor/bme680/gasPercentage"         , 1, true, String(data_bme.gasPercentage).c_str());
          mqttClient.publish("sensor/bme680/gasPercentageAccuracy" , 1, true, String(data_bme.gasPercentageAccuracy).c_str());
          mqttClient.publish("sensor/bme680/iaq"                   , 1, true, String(data_bme.iaq).c_str());
          mqttClient.publish("sensor/bme680/iaqAccuracy"           , 1, true, String(data_bme.iaqAccuracy).c_str());
          mqttClient.publish("sensor/bme680/staticIaqAccuracy"     , 1, true, String(data_bme.staticIaqAccuracy).c_str());

          mqttClient.publish("sensor/mhz19/Accuracy"       , 1, true, String(data_co2.getAccuracy()).c_str());
          mqttClient.publish("sensor/mhz19/Background"     , 1, true, String(data_co2.getBackground()).c_str());
          mqttClient.publish("sensor/mhz19/Limited"        , 1, true, String(data_co2.getLimited()).c_str());
          mqttClient.publish("sensor/mhz19/Raw"            , 1, true, String(data_co2.getRaw()).c_str());
          mqttClient.publish("sensor/mhz19/Regular"        , 1, true, String(data_co2.getRegular()).c_str());
          mqttClient.publish("sensor/mhz19/TempAdjustment" , 1, true, String(data_co2.getTempAdjustment()).c_str());
          mqttClient.publish("sensor/mhz19/Temperature"    , 1, true, String(data_co2.getTemperature()).c_str()); 

          #ifdef DEBUG
            Serial.println("[MQTT] Send data");
          #endif
        }
        else{
          #ifdef DEBUG
            Serial.println("[MQTT] No data send");
          #endif

        }

        disconnectfromMqtt();

    }
}
