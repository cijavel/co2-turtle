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


<<<<<<< HEAD
=======
void MqttClientHandler::onMqttConnect(bool sessionPresent) {


  //Serial.println("Connected to MQTT.");
  //Serial.print("Session present: ");
  // Serial.println(sessionPresent);
  //uint16_t packetIdSub = mqttClient.subscribe("test/lol", 2);
  //Serial.print("Subscribing at QoS 2, packetId: ");
  //Serial.println(packetIdSub);
  //mqttClient.publish("test/lol", 0, true, "ping");
  //Serial.println("Publishing at QoS 0");
  //uint16_t packetIdPub1 = mqttClient.publish("test/lol", 1, true, "test 2");
  //Serial.print("Publishing at QoS 1, packetId: ");
  //Serial.println(packetIdPub1);
  //uint16_t packetIdPub2 = mqttClient.publish("test/lol", 2, true, "test 3");
  //Serial.print("Publishing at QoS 2, packetId: ");
  //Serial.println(packetIdPub2);
}

>>>>>>> 9bd3090 (minor: mqtt works)
void MqttClientHandler::onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("[MQTT] Disconnected");
}

<<<<<<< HEAD
=======
void MqttClientHandler::onMqttSubscribe(uint16_t packetId, uint8_t qos) {
 //Serial.println("[MQTT] Subscribe acknowledged.");
 //Serial.print("  packetId: ");
 //Serial.println(packetId);
 //Serial.print("  qos: ");
 //Serial.println(qos);
}

void MqttClientHandler::onMqttUnsubscribe(uint16_t packetId) {
  //Serial.println("[MQTT] Unsubscribe acknowledged.");
  //Serial.print("  packetId: ");
  //Serial.println(packetId);
}

void MqttClientHandler::onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  //Serial.println("[MQTT] Publish received.");
  //Serial.print("  topic: ");
  //Serial.println(topic);
  //Serial.print("  qos: ");
  //Serial.println(properties.qos);
  //Serial.print("  dup: ");
  //Serial.println(properties.dup);
  //Serial.print("  retain: ");
  //Serial.println(properties.retain);
  //Serial.print("  len: ");
  //Serial.println(len);
  //Serial.print("  index: ");
  //Serial.println(index);
  //Serial.print("  total: ");
  //Serial.println(total);
}

void MqttClientHandler::onMqttPublish(uint16_t packetId) {
  Serial.println("[MQTT] Publish acknowledged");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}
>>>>>>> 9bd3090 (minor: mqtt works)

void MqttClientHandler::setup_Mqtt() {
  WiFi.onEvent(WiFiEvent);

  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCredentials(MQTT_USER, MQTT_PASS);
  Serial.println("[MQTT] connected");
}



void MqttClientHandler::publishData(const DataCO2 data_co2, const Bsec data_bme, const unsigned long currentSeconds){
    if (currentSeconds % interval_mqtt_in_Seconds == 0){

<<<<<<< HEAD
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
=======
    
    Serial.println("[MQTT] connected 3 ");
    Serial.println(String(mqttClient.connected()));


    connectToMqtt();

    Serial.println("[MQTT] connected after 4 ");
    Serial.println(String(mqttClient.connected()));

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
    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/temperature          " , packetIdPub01);
    Serial.printf("Message: %s  \n", String(data_bme.temperature).c_str());

    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/temperature_raw      "  , packetIdPub02);
    Serial.printf("Message: %s  \n", String(data_bme.rawTemperature).c_str());
  
    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/humidity             "  , packetIdPub03);
    Serial.printf("Message: %s  \n", String(data_bme.humidity).c_str());

    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/humidity_raw         "  , packetIdPub04);
    Serial.printf("Message: %s  \n", String(data_bme.rawHumidity).c_str()); 

    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/pressure             "  , packetIdPub05);
    Serial.printf("Message: %s  \n", String(data_bme.pressure).c_str());

    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/gas                  "  , packetIdPub06);
    Serial.printf("Message: %s  \n", String(data_bme.gasResistance).c_str());

    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/bme68xStatus         "  , packetIdPub07);
    Serial.printf("Message: %s  \n", String(data_bme.bme68xStatus).c_str());

    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/breathVocAccuracy    "  , packetIdPub08);
    Serial.printf("Message: %s  \n", String(data_bme.breathVocAccuracy).c_str());

    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/breathVocEquivalent  "  , packetIdPub09);
    Serial.printf("Message: %s  \n", String(data_bme.breathVocEquivalent).c_str());

    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/bsecStatus           "  , packetIdPub10);
    Serial.printf("Message: %s  \n", String(data_bme.bsecStatus).c_str());

    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/co2Accuracy          "  , packetIdPub11);
    Serial.printf("Message: %s  \n", String(data_bme.co2Accuracy).c_str());

    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/co2Equivalent        "  , packetIdPub12);
    Serial.printf("Message: %s  \n", String(data_bme.co2Equivalent).c_str());

    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/compGasAccuracy      "  , packetIdPub13);
    Serial.printf("Message: %s \n", String(data_bme.compGasAccuracy).c_str());

    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/compGasValue         "  , packetIdPub14);
    Serial.printf("Message: %s  \n", String(data_bme.compGasValue).c_str());

    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/gasPercentage        "  , packetIdPub15);
    Serial.printf("Message: %s  \n", String(data_bme.gasPercentage).c_str());

    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/gasPercentageAccuracy"  , packetIdPub16);
    Serial.printf("Message: %s   \n", String(data_bme.gasPercentageAccuracy).c_str());

    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/iaq                  "  , packetIdPub17);
    Serial.printf("Message: %s  \n", String(data_bme.iaq).c_str());
    
    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/iaqAccuracy          "  , packetIdPub18);
    Serial.printf("Message: %s  \n", String(data_bme.iaqAccuracy).c_str());

    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/bme680/staticIaqAccuracy    "  , packetIdPub19);
    Serial.printf("Message: %s  \n", String(data_bme.staticIaqAccuracy).c_str());
																										 
    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/mhz19/Accuracy              "  , packetIdPub20);
    Serial.printf("Message: %s  \n", String(data_co2.getAccuracy()).c_str());

    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/mhz19/Background            "  , packetIdPub21);
    Serial.printf("Message: %s  \n", String(data_co2.getBackground()).c_str());
 
    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/mhz19/Limited               "  , packetIdPub22);
    Serial.printf("Message: %s  \n", String(data_co2.getLimited()).c_str());

    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/mhz19/Raw                   "  , packetIdPub23);
    Serial.printf("Message: %s  \n", String(data_co2.getRaw()).c_str());

    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/mhz19/Regular               " , packetIdPub24);
    Serial.printf("Message: %s  \n", String(data_co2.getRegular()).c_str());

    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/mhz19/TempAdjustment        " , packetIdPub25);
    Serial.printf("Message: %s  \n", String(data_co2.getTempAdjustment()).c_str());

    Serial.printf("[MQTT] Publishing on topic %s at QoS 1, packetId: %i ,", "sensor/mhz19/Temperature           " , packetIdPub26);
    Serial.printf("Message: %s  \n", String(data_co2.getTemperature()).c_str());
  #endif
>>>>>>> 9bd3090 (minor: mqtt works)

    }
}
