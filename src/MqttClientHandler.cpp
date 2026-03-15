#include "MqttClientHandler.h"
#include "Configuration.h"
#include "Credentials.h"
#include <WiFi.h>
#include "ConfigHandler.h"
#include "BME680Handler.h"
#include <AsyncMqttClient.h>
unsigned long MqttClientHandler::_lastRunSeconds = 0;
extern ConfigHandler &configHandler;
extern "C"
{
    #include "freertos/FreeRTOS.h"
    #include "freertos/timers.h"
}

AsyncMqttClient mqttClient;
String MqttClientHandler::_deviceId;
String MqttClientHandler::_topicBase;

void MqttClientHandler::connectToMqtt()
{
	if (mqttClient.connected() == false)
	{
		mqttClient.connect();
	}
}

bool MqttClientHandler::isConnected()
{
    return mqttClient.connected();
}

void MqttClientHandler::WiFiEvent(WiFiEvent_t event)
{
	#if DEBUG
		Serial.printf("[MQTT] WiFiEvent %d, status=%d\n", event, (int)WiFiClass::status());
	#endif
    if (WiFiClass::status() != WL_CONNECTED)
    {
        Serial.println("[MQTT] WiFi lost – skipping reconnect");
    }
    else
    {
        Serial.println("[MQTT] WiFi up, connecting to MQTT...");
        connectToMqtt();
    }
}

void MqttClientHandler::onMqttConnect(bool sessionPresent)
{
	Serial.println("[MQTT] Connected, publishing discovery...");
	publishDiscovery();
}

void MqttClientHandler::onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
    const char* reasonStr = "unknown";
    switch (reason) {
        case AsyncMqttClientDisconnectReason::TCP_DISCONNECTED:                   reasonStr = "TCP disconnected"; break;
        case AsyncMqttClientDisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION: reasonStr = "bad protocol version"; break;
        case AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED:           reasonStr = "client ID rejected"; break;
        case AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE:            reasonStr = "server unavailable"; break;
        case AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS:         reasonStr = "bad credentials"; break;
        case AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED:                reasonStr = "not authorized"; break;
        default: break;
    }
    Serial.printf("[MQTT] Disconnected: %s (code %d)\n", reasonStr, (int)reason);
}

void MqttClientHandler::setup_Mqtt()
{
	WiFi.onEvent(WiFiEvent);

	mqttClient.onConnect(onMqttConnect);
	mqttClient.onDisconnect(onMqttDisconnect);

	// Build unique device ID and topic base from NVS deviceName
	_deviceId = configHandler.getConfigDevice("deviceName");
	_deviceId.replace(" ", "-");
	_deviceId.toLowerCase();
	_topicBase = "homeassistant/sensor/" + _deviceId;
	mqttClient.setClientId(_deviceId.c_str());
	Serial.println("[MQTT] Client-ID: " + _deviceId);

	_mqttHost = configHandler.getConfigDevice("mqttHOST");
	int port = configHandler.getConfigDevice("mqttPORT").toInt();
	mqttClient.setServer(_mqttHost.c_str(), port);

	if (configHandler.getConfigDevice("mqttUSERen") == "1")
	{
		_mqttUser = configHandler.getConfigDevice("mqttUSER");
		_mqttPass = configHandler.getConfigDevice("mqttPASSWORD");
		mqttClient.setCredentials(_mqttUser.c_str(), _mqttPass.c_str());
	}
	Serial.println("[MQTT] setup done, server: " + _mqttHost + ":" + String(port));
	_isSetup = true;

	// Only connect if WiFi is already up; otherwise WiFiEvent will trigger connect
	if (WiFi.status() == WL_CONNECTED)
	{
		connectToMqtt();
	}
}

void MqttClientHandler::publishDiscovery()
{
    if (!mqttClient.connected()) return;

    String deviceBME = "\"device\":{\"identifiers\":[\"" + _deviceId + "_bme680\"],\"name\":\"" + _deviceId + " BME680\",\"model\":\"BME680\",\"manufacturer\":\"Bosch\",\"via_device\":\"" + _deviceId + "\"}";
    String deviceMHZ = "\"device\":{\"identifiers\":[\"" + _deviceId + "_mhz19\"],\"name\":\"" + _deviceId + " MHZ19\",\"model\":\"MH-Z19B\",\"manufacturer\":\"Winsen\",\"via_device\":\"" + _deviceId + "\"}";

    struct SensorConfig {
        const char* uniqueId;
        const char* name;
        const char* unit;
        const char* deviceClass;
        const char* stateClass;
        bool isBME;
    };

    SensorConfig sensors[] = {
        // BME680
        {"temperature",         "Temperature",          "°C",  "temperature",    "measurement", true},
        {"temperature_offset",  "Temperature (Offset)", "°C",  "temperature",    "measurement", true},
        {"humidity",            "Humidity",             "%",   "humidity",       "measurement", true},
        {"pressure",            "Air Pressure",         "hPa", "pressure",       "measurement", true},
        {"gas",                 "Gas Resistance",       "Ω",   "",               "measurement", true},
        {"iaq",                 "IAQ",                  "",    "",               "measurement", true},
        {"iaq_accuracy",        "IAQ Accuracy",         "",    "",               "measurement", true},
        {"breath_voc",          "Breath VOC",           "ppm", "",               "measurement", true},
        {"co2_equivalent",      "CO2 Equivalent",       "ppm", "carbon_dioxide", "measurement", true},
        {"static_iaq_accuracy", "Static IAQ Accuracy",  "",    "",               "measurement", true},
        {"comp_gas_value",      "Comp Gas Value",       "",    "",               "measurement", true},
        {"gas_percentage",      "Gas Percentage",       "%",   "",               "measurement", true},
        // MHZ19
        {"co2",                 "CO2",                  "ppm", "carbon_dioxide", "measurement", false},
        {"co2_raw",             "CO2 Raw",              "ppm", "carbon_dioxide", "measurement", false},
        {"co2_accuracy",        "CO2 Accuracy",         "",    "",               "measurement", false},
        {"co2_limited",         "CO2 Limited",          "ppm", "carbon_dioxide", "measurement", false},
        {"co2_background",      "CO2 Background",       "ppm", "carbon_dioxide", "measurement", false},
        {"co2_temp_adjustment", "CO2 Temp Adjustment",  "",    "",               "measurement", false},
        {"co2_temperature",     "CO2 Temperature",      "°C",  "temperature",    "measurement", false},
    };

    for (auto& s : sensors)
    {
        String configTopic = _topicBase + "/" + s.uniqueId + "/config";
        String stateTopic  = _topicBase + "/" + s.uniqueId + "/state";

        String payload = "{";
        payload += "\"unique_id\":\"" + _deviceId + "_" + s.uniqueId + "\",";
        payload += "\"name\":\"" + String(s.name) + "\",";
        payload += "\"state_topic\":\"" + stateTopic + "\",";
        if (strlen(s.unit) > 0) payload += "\"unit_of_measurement\":\"" + String(s.unit) + "\",";
        if (strlen(s.deviceClass) > 0) payload += "\"device_class\":\"" + String(s.deviceClass) + "\",";
        payload += "\"state_class\":\"" + String(s.stateClass) + "\",";
        payload += (s.isBME ? deviceBME : deviceMHZ);
        payload += "}";

        mqttClient.publish(configTopic.c_str(), 1, true, payload.c_str());
    }

    Serial.println("[MQTT] Discovery published for " + _deviceId);
}

void MqttClientHandler::publishData(const DataCO2 data_co2, const Bsec data_bme, const unsigned long currentSeconds)
{
	if (currentSeconds - _lastRunSeconds >= (unsigned long)configHandler.getConfigInterval("intervalMQTT"))
	{
		_lastRunSeconds = currentSeconds;
		connectToMqtt();
		if (mqttClient.connected() == true)
		{
            bool bmeOk = BME680Handler::getInstance().isSensorOk();
			if (bmeOk)
			{
                // BME680
                mqttClient.publish((_topicBase + "/temperature/state").c_str(),         1, true, String(data_bme.temperature).c_str());
                mqttClient.publish((_topicBase + "/temperature_offset/state").c_str(),  1, true, String(data_bme.temperature + configHandler.getConfigSensor("tempOffset") / 10.0f).c_str());
                mqttClient.publish((_topicBase + "/humidity/state").c_str(),            1, true, String(data_bme.humidity).c_str());
                mqttClient.publish((_topicBase + "/pressure/state").c_str(),            1, true, String(data_bme.pressure / 100.0f).c_str());
                mqttClient.publish((_topicBase + "/gas/state").c_str(),                 1, true, String(data_bme.gasResistance).c_str());
                mqttClient.publish((_topicBase + "/iaq/state").c_str(),                 1, true, String(data_bme.iaq).c_str());
                mqttClient.publish((_topicBase + "/iaq_accuracy/state").c_str(),        1, true, String(data_bme.iaqAccuracy).c_str());
                mqttClient.publish((_topicBase + "/breath_voc/state").c_str(),          1, true, String(data_bme.breathVocEquivalent).c_str());
                mqttClient.publish((_topicBase + "/co2_equivalent/state").c_str(),      1, true, String(data_bme.co2Equivalent).c_str());
                mqttClient.publish((_topicBase + "/static_iaq_accuracy/state").c_str(), 1, true, String(data_bme.staticIaqAccuracy).c_str());
                mqttClient.publish((_topicBase + "/comp_gas_value/state").c_str(),      1, true, String(data_bme.compGasValue).c_str());
                mqttClient.publish((_topicBase + "/gas_percentage/state").c_str(),      1, true, String(data_bme.gasPercentage).c_str());
            } // bmeOk

			// MHZ19
			mqttClient.publish((_topicBase + "/co2/state").c_str(),                 1, true, String(data_co2.getRegular()).c_str());
			mqttClient.publish((_topicBase + "/co2_raw/state").c_str(),             1, true, String(data_co2.getRaw()).c_str());
			mqttClient.publish((_topicBase + "/co2_accuracy/state").c_str(),        1, true, String(data_co2.getAccuracy()).c_str());
			mqttClient.publish((_topicBase + "/co2_limited/state").c_str(),         1, true, String(data_co2.getLimited()).c_str());
			mqttClient.publish((_topicBase + "/co2_background/state").c_str(),      1, true, String(data_co2.getBackground()).c_str());
			mqttClient.publish((_topicBase + "/co2_temp_adjustment/state").c_str(), 1, true, String(data_co2.getTempAdjustment()).c_str());
			mqttClient.publish((_topicBase + "/co2_temperature/state").c_str(),     1, true, String(data_co2.getTemperature()).c_str());

			#if DEBUG
				Serial.println("[MQTT] Send data");
		    #endif
		}
		else
		{
			#if DEBUG
				Serial.printf("[MQTT] Publish skipped – not connected (state: %s)\n", mqttClient.connected() ? "connected" : "disconnected");
			#endif
		}
	}
}
