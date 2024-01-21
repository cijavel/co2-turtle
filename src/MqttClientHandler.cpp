#include "MqttClientHandler.h"
#include "Configuration.h"

#if ASYNC_TCP_SSL_ENABLED
  #define MQTT_SECURE true
  #define MQTT_SERVER_FINGERPRINT {0x7e, 0x36, 0x22, 0x01, 0xf9, 0x7e, 0x99, 0x2f, 0xc5, 0xdb, 0x3d, 0xbe, 0xac, 0x48, 0x67, 0x5b, 0x5d, 0x47, 0x94, 0xd2}
  #define MQTT_PORT 8883
  #else
  #define MQTT_PORT 1883
#endif

AsyncMqttClient mqttClient;

extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/timers.h"
}

TimerHandle_t mqttReconnectTimer;

void MqttClientHandler::connectToMqtt() {
  Serial.println("MQTT: Connecting to MQTT.");
  mqttClient.connect();
}

void MqttClientHandler::WiFiEvent(WiFiEvent_t event) {
    Serial.printf("[WiFi-event] event: %d\n", event);
    wl_status_t status = WiFiClass::status();
    if (status != WL_CONNECTED)
    {
        Serial.println("MQTT: WiFi lost connection");
        xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    }
    else {
        connectToMqtt();
    }
}

void MqttClientHandler::onMqttConnect(bool sessionPresent) {
  Serial.println("MQTT: Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

void MqttClientHandler::onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("MQTT: Disconnected from MQTT.");
  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void MqttClientHandler::onMqttPublish(uint16_t packetId) {
  Serial.print("MQTT: Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}


void MqttClientHandler::setup_Mqtt(){

    mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
    mqttClient.setKeepAlive(interval_mqtt_in_Seconds);
    WiFi.onEvent(WiFiEvent);


    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    //mqttClient.onSubscribe(onMqttSubscribe);
    //mqttClient.onUnsubscribe(onMqttUnsubscribe);
    mqttClient.onPublish(onMqttPublish);
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    // If your broker requires authentication (username and password), set them below
    mqttClient.setCredentials("Sensorturtle", "FfGCfePxCqpIOmZHdkNX");


    #if ASYNC_TCP_SSL_ENABLED
      mqttClient.setSecure(MQTT_SECURE);
      if (MQTT_SECURE) {
      mqttClient.addServerFingerprint((const uint8_t[])MQTT_SERVER_FINGERPRINT);
      }
    #endif

    #ifdef DEBUG
            Serial.println("MQTT: Set MQTT configuration");
            Serial.println("MQTT: clientID: " + String(mqttClient.getClientId()));
            mqttClient.connected();
          
    #endif

}


void MqttClientHandler::publishData(const CO2Data data_co2, const Bsec data_bme, const unsigned long currentSeconds){
    if (currentSeconds % interval_mqtt_in_Seconds == 0){

    uint16_t packetIdPub01 = mqttClient.publish("sensor/bme680/temperature"           , 1, true, String(data_bme.temperature).c_str());
    uint16_t packetIdPub02 = mqttClient.publish("sensor/bme680/temperature_raw"       , 1, true, String(data_bme.rawTemperature).c_str());
    uint16_t packetIdPub03 = mqttClient.publish("sensor/bme680/humidity"              , 1, true, String(data_bme.humidity).c_str());
    uint16_t packetIdPub04 = mqttClient.publish("sensor/bme680/humidity_raw"          , 1, true, String(data_bme.rawHumidity).c_str());
    uint16_t packetIdPub05 = mqttClient.publish("sensor/bme680/pressure"              , 1, true, String(data_bme.pressure).c_str());
    uint16_t packetIdPub06 = mqttClient.publish("sensor/bme680/gas"                   , 1, true, String(data_bme.gasResistance).c_str());
    uint16_t packetIdPub07 = mqttClient.publish("sensor/bme680/bme68xStatus"          , 1, true, String(data_bme.bme68xStatus).c_str());
    uint16_t packetIdPub08 = mqttClient.publish("sensor/bme680/breathVocAccuracy"     , 1, true, String(data_bme.breathVocAccuracy).c_str());
    uint16_t packetIdPub09 = mqttClient.publish("sensor/bme680/breathVocEquivalent"   , 1, true, String(data_bme.breathVocEquivalent).c_str());
    uint16_t packetIdPub10 = mqttClient.publish("sensor/bme680/bsecStatus"            , 1, true, String(data_bme.bsecStatus).c_str());
    uint16_t packetIdPub11 = mqttClient.publish("sensor/bme680/co2Accuracy"           , 1, true, String(data_bme.co2Accuracy).c_str());
    uint16_t packetIdPub12 = mqttClient.publish("sensor/bme680/co2Equivalent"         , 1, true, String(data_bme.co2Equivalent).c_str());
    uint16_t packetIdPub13 = mqttClient.publish("sensor/bme680/compGasAccuracy"       , 1, true, String(data_bme.compGasAccuracy).c_str());
    uint16_t packetIdPub14 = mqttClient.publish("sensor/bme680/compGasValue"          , 1, true, String(data_bme.compGasValue).c_str());
    uint16_t packetIdPub15 = mqttClient.publish("sensor/bme680/gasPercentage"         , 1, true, String(data_bme.gasPercentage).c_str());
    uint16_t packetIdPub16 = mqttClient.publish("sensor/bme680/gasPercentageAccuracy" , 1, true, String(data_bme.gasPercentageAccuracy).c_str());
    uint16_t packetIdPub17 = mqttClient.publish("sensor/bme680/iaq"                   , 1, true, String(data_bme.iaq).c_str());
    uint16_t packetIdPub18 = mqttClient.publish("sensor/bme680/iaqAccuracy"           , 1, true, String(data_bme.iaqAccuracy).c_str());
    uint16_t packetIdPub19 = mqttClient.publish("sensor/bme680/staticIaqAccuracy"     , 1, true, String(data_bme.staticIaqAccuracy).c_str());
   
    uint16_t packetIdPub20 = mqttClient.publish("sensor/mhz19/Accuracy"       , 1, true, String(data_co2.getAccuracy()).c_str());
    uint16_t packetIdPub21 = mqttClient.publish("sensor/mhz19/Background"     , 1, true, String(data_co2.getBackground()).c_str());
    uint16_t packetIdPub22 = mqttClient.publish("sensor/mhz19/Limited"        , 1, true, String(data_co2.getLimited()).c_str());
    uint16_t packetIdPub23 = mqttClient.publish("sensor/mhz19/Raw"            , 1, true, String(data_co2.getRaw()).c_str());
    uint16_t packetIdPub24 = mqttClient.publish("sensor/mhz19/Regular"        , 1, true, String(data_co2.getRegular()).c_str());
    uint16_t packetIdPub25 = mqttClient.publish("sensor/mhz19/TempAdjustment" , 1, true, String(data_co2.getTempAdjustment()).c_str());
    uint16_t packetIdPub26 = mqttClient.publish("sensor/mhz19/Temperature"    , 1, true, String(data_co2.getTemperature()).c_str());


  #ifdef DEBUG 
    Serial.println();
    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/temperature          " , packetIdPub01);
    Serial.printf("Message: %s  \n", String(data_bme.temperature).c_str());

    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/temperature_raw      "  , packetIdPub02);
    Serial.printf("Message: %s  \n", String(data_bme.rawTemperature).c_str());
  
    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/humidity             "  , packetIdPub03);
    Serial.printf("Message: %s  \n", String(data_bme.humidity).c_str());

    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/humidity_raw         "  , packetIdPub04);
    Serial.printf("Message: %s  \n", String(data_bme.rawHumidity).c_str()); 

    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/pressure             "  , packetIdPub05);
    Serial.printf("Message: %s  \n", String(data_bme.pressure).c_str());

    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/gas                  "  , packetIdPub06);
    Serial.printf("Message: %s  \n", String(data_bme.gasResistance).c_str());

    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/bme68xStatus         "  , packetIdPub07);
    Serial.printf("Message: %s  \n", String(data_bme.bme68xStatus).c_str());

    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/breathVocAccuracy    "  , packetIdPub08);
    Serial.printf("Message: %s  \n", String(data_bme.breathVocAccuracy).c_str());

    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/breathVocEquivalent  "  , packetIdPub09);
    Serial.printf("Message: %s  \n", String(data_bme.breathVocEquivalent).c_str());

    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/bsecStatus           "  , packetIdPub10);
    Serial.printf("Message: %s  \n", String(data_bme.bsecStatus).c_str());

    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/co2Accuracy          "  , packetIdPub11);
    Serial.printf("Message: %s  \n", String(data_bme.co2Accuracy).c_str());

    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/co2Equivalent        "  , packetIdPub12);
    Serial.printf("Message: %s  \n", String(data_bme.co2Equivalent).c_str());

    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/compGasAccuracy      "  , packetIdPub13);
    Serial.printf("Message: %s \n", String(data_bme.compGasAccuracy).c_str());

    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/compGasValue         "  , packetIdPub14);
    Serial.printf("Message: %s  \n", String(data_bme.compGasValue).c_str());

    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/gasPercentage        "  , packetIdPub15);
    Serial.printf("Message: %s  \n", String(data_bme.gasPercentage).c_str());

    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/gasPercentageAccuracy"  , packetIdPub16);
    Serial.printf("Message: %s   \n", String(data_bme.gasPercentageAccuracy).c_str());

    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/iaq                  "  , packetIdPub17);
    Serial.printf("Message: %s  \n", String(data_bme.iaq).c_str());
    
    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/iaqAccuracy          "  , packetIdPub18);
    Serial.printf("Message: %s  \n", String(data_bme.iaqAccuracy).c_str());

    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/staticIaqAccuracy    "  , packetIdPub19);
    Serial.printf("Message: %s  \n", String(data_bme.staticIaqAccuracy).c_str());
																										 
    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/mhz19/Accuracy              "  , packetIdPub20);
    Serial.printf("Message: %s  \n", String(data_co2.getAccuracy()).c_str());

    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/mhz19/Background            "  , packetIdPub21);
    Serial.printf("Message: %s  \n", String(data_co2.getBackground()).c_str());
 
    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/mhz19/Limited               "  , packetIdPub22);
    Serial.printf("Message: %s  \n", String(data_co2.getLimited()).c_str());

    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/mhz19/Raw                   "  , packetIdPub23);
    Serial.printf("Message: %s  \n", String(data_co2.getRaw()).c_str());

    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/mhz19/Regular               " , packetIdPub24);
    Serial.printf("Message: %s  \n", String(data_co2.getRegular()).c_str());

    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/mhz19/TempAdjustment        " , packetIdPub25);
    Serial.printf("Message: %s  \n", String(data_co2.getTempAdjustment()).c_str());

    Serial.printf("MQTT: Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/mhz19/Temperature           " , packetIdPub26);
    Serial.printf("Message: %s  \n", String(data_co2.getTemperature()).c_str());
  #endif

    }
}



