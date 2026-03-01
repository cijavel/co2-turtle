#include "MqttClientHandler.h"
#include "Configuration.h"
#include "Credentials.h"
#include <WiFi.h>
#include "ConfigHandler.h"
unsigned long MqttClientHandler::_lastRunSeconds = 0;
extern ConfigHandler &configHandler;

extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>
AsyncMqttClient mqttClient;

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
	Serial.printf("[MQTT] event: %d\n", event);
	wl_status_t status = WiFiClass::status();
	if (status != WL_CONNECTED)
	{
		Serial.println("[MQTT] lost connection");
	}
	else
	{
		Serial.println("[MQTT] reconnection");
		connectToMqtt();
	}
}

void MqttClientHandler::onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
	Serial.println("[MQTT] Disconnected");
}

void MqttClientHandler::setup_Mqtt()
{
	WiFi.onEvent(WiFiEvent);

	mqttClient.onDisconnect(onMqttDisconnect);

	String host = configHandler.getConfigDevice("mqttHOST");
	int port = configHandler.getConfigDevice("mqttPORT").toInt();
	mqttClient.setServer(host.c_str(), port);

	if (configHandler.getConfigDevice("mqttUSERen") == "1")
	{
		mqttClient.setCredentials(
			configHandler.getConfigDevice("mqttUSER").c_str(),
			configHandler.getConfigDevice("mqttPASSWORD").c_str()
		);
	}
	Serial.println("[MQTT] setup done, server: " + host + ":" + String(port));
}

void MqttClientHandler::publishDiscovery()
{
    if (!mqttClient.connected()) return;

    String deviceBME = "\"device\":{\"identifiers\":[\"sensorturtle_bme680\"],\"name\":\"SensorTurtle BME680\",\"model\":\"BME680\",\"manufacturer\":\"Bosch\",\"via_device\":\"sensorturtle\"}";
    String deviceMHZ = "\"device\":{\"identifiers\":[\"sensorturtle_mhz19\"],\"name\":\"SensorTurtle MHZ19\",\"model\":\"MH-Z19B\",\"manufacturer\":\"Winsen\",\"via_device\":\"sensorturtle\"}";

    struct SensorConfig {
        const char* uniqueId;
        const char* name;
        const char* stateTopic;
        const char* unit;
        const char* deviceClass;
        const char* stateClass;
        bool isBME;
    };

    SensorConfig sensors[] = {
        // BME680
        {"temperature",         "Temperature",          "homeassistant/sensor/sensorturtle/temperature/state",          "°C",  "temperature",    "measurement", true},
        {"temperature_offset",  "Temperature (Offset)", "homeassistant/sensor/sensorturtle/temperature_offset/state",   "°C",  "temperature",    "measurement", true},
        {"humidity",            "Humidity",             "homeassistant/sensor/sensorturtle/humidity/state",             "%",   "humidity",       "measurement", true},
        {"pressure",            "Air Pressure",         "homeassistant/sensor/sensorturtle/pressure/state",             "hPa", "pressure",       "measurement", true},
        {"gas",                 "Gas Resistance",       "homeassistant/sensor/sensorturtle/gas/state",                  "Ω",   "",               "measurement", true},
        {"iaq",                 "IAQ",                  "homeassistant/sensor/sensorturtle/iaq/state",                  "",    "",               "measurement", true},
        {"iaq_accuracy",        "IAQ Accuracy",         "homeassistant/sensor/sensorturtle/iaq_accuracy/state",         "",    "",               "measurement", true},
        {"breath_voc",          "Breath VOC",           "homeassistant/sensor/sensorturtle/breath_voc/state",           "ppm", "",               "measurement", true},
        {"co2_equivalent",      "CO2 Equivalent",       "homeassistant/sensor/sensorturtle/co2_equivalent/state",       "ppm", "carbon_dioxide", "measurement", true},
        {"static_iaq_accuracy", "Static IAQ Accuracy",  "homeassistant/sensor/sensorturtle/static_iaq_accuracy/state",  "",    "",               "measurement", true},
        {"comp_gas_value",      "Comp Gas Value",       "homeassistant/sensor/sensorturtle/comp_gas_value/state",       "",    "",               "measurement", true},
        {"gas_percentage",      "Gas Percentage",       "homeassistant/sensor/sensorturtle/gas_percentage/state",       "%",   "",               "measurement", true},
        // MHZ19
        {"co2",                 "CO2",                  "homeassistant/sensor/sensorturtle/co2/state",                  "ppm", "carbon_dioxide", "measurement", false},
        {"co2_raw",             "CO2 Raw",              "homeassistant/sensor/sensorturtle/co2_raw/state",              "ppm", "carbon_dioxide", "measurement", false},
        {"co2_accuracy",        "CO2 Accuracy",         "homeassistant/sensor/sensorturtle/co2_accuracy/state",         "",    "",               "measurement", false},
        {"co2_limited",         "CO2 Limited",          "homeassistant/sensor/sensorturtle/co2_limited/state",          "ppm", "carbon_dioxide", "measurement", false},
        {"co2_background",      "CO2 Background",       "homeassistant/sensor/sensorturtle/co2_background/state",       "ppm", "carbon_dioxide", "measurement", false},
        {"co2_temp_adjustment", "CO2 Temp Adjustment",  "homeassistant/sensor/sensorturtle/co2_temp_adjustment/state",  "",    "",               "measurement", false},
        {"co2_temperature",     "CO2 Temperature",      "homeassistant/sensor/sensorturtle/co2_temperature/state",      "°C",  "temperature",    "measurement", false},
    };

    for (auto& s : sensors)
    {
        String configTopic = "homeassistant/sensor/sensorturtle/";
        configTopic += s.uniqueId;
        configTopic += "/config";

        String payload = "{";
        payload += "\"unique_id\":\"sensorturtle_" + String(s.uniqueId) + "\",";
        payload += "\"name\":\"" + String(s.name) + "\",";
        payload += "\"state_topic\":\"" + String(s.stateTopic) + "\",";
        if (strlen(s.unit) > 0)
            payload += "\"unit_of_measurement\":\"" + String(s.unit) + "\",";
        if (strlen(s.deviceClass) > 0)
            payload += "\"device_class\":\"" + String(s.deviceClass) + "\",";
        payload += "\"state_class\":\"" + String(s.stateClass) + "\",";
        payload += (s.isBME ? deviceBME : deviceMHZ);
        payload += "}";

        mqttClient.publish(configTopic.c_str(), 1, true, payload.c_str());
    }

    Serial.println("[MQTT] Discovery published");
}

void MqttClientHandler::publishData(const DataCO2 data_co2, const Bsec data_bme, const unsigned long currentSeconds)
{
	if (currentSeconds - _lastRunSeconds >= (unsigned long)configHandler.getConfigInterval("intervalMQTT"))
	{
		_lastRunSeconds = currentSeconds;
		connectToMqtt();
		if (mqttClient.connected() == true)
		{
			// BME680
			mqttClient.publish("homeassistant/sensor/sensorturtle/temperature/state",         1, true, String(data_bme.temperature).c_str());
			mqttClient.publish("homeassistant/sensor/sensorturtle/temperature_offset/state",  1, true, String(data_bme.temperature + configHandler.getConfigSensor("tempOffset") / 10.0f).c_str());
			mqttClient.publish("homeassistant/sensor/sensorturtle/humidity/state",            1, true, String(data_bme.humidity).c_str());
			mqttClient.publish("homeassistant/sensor/sensorturtle/pressure/state",            1, true, String(data_bme.pressure / 100.0f).c_str());
			mqttClient.publish("homeassistant/sensor/sensorturtle/gas/state",                 1, true, String(data_bme.gasResistance).c_str());
			mqttClient.publish("homeassistant/sensor/sensorturtle/iaq/state",                 1, true, String(data_bme.iaq).c_str());
			mqttClient.publish("homeassistant/sensor/sensorturtle/iaq_accuracy/state",        1, true, String(data_bme.iaqAccuracy).c_str());
			mqttClient.publish("homeassistant/sensor/sensorturtle/breath_voc/state",          1, true, String(data_bme.breathVocEquivalent).c_str());
			mqttClient.publish("homeassistant/sensor/sensorturtle/co2_equivalent/state",      1, true, String(data_bme.co2Equivalent).c_str());
			mqttClient.publish("homeassistant/sensor/sensorturtle/static_iaq_accuracy/state", 1, true, String(data_bme.staticIaqAccuracy).c_str());
			mqttClient.publish("homeassistant/sensor/sensorturtle/comp_gas_value/state",      1, true, String(data_bme.compGasValue).c_str());
			mqttClient.publish("homeassistant/sensor/sensorturtle/gas_percentage/state",      1, true, String(data_bme.gasPercentage).c_str());

			// MHZ19
			mqttClient.publish("homeassistant/sensor/sensorturtle/co2/state",                 1, true, String(data_co2.getRegular()).c_str());
			mqttClient.publish("homeassistant/sensor/sensorturtle/co2_raw/state",             1, true, String(data_co2.getRaw()).c_str());
			mqttClient.publish("homeassistant/sensor/sensorturtle/co2_accuracy/state",        1, true, String(data_co2.getAccuracy()).c_str());
			mqttClient.publish("homeassistant/sensor/sensorturtle/co2_limited/state",         1, true, String(data_co2.getLimited()).c_str());
			mqttClient.publish("homeassistant/sensor/sensorturtle/co2_background/state",      1, true, String(data_co2.getBackground()).c_str());
			mqttClient.publish("homeassistant/sensor/sensorturtle/co2_temp_adjustment/state", 1, true, String(data_co2.getTempAdjustment()).c_str());
			mqttClient.publish("homeassistant/sensor/sensorturtle/co2_temperature/state",     1, true, String(data_co2.getTemperature()).c_str());

#ifdef DEBUG
			Serial.println("[MQTT] Send data");
#endif
		}
		else
		{
#ifdef DEBUG
			Serial.println("[MQTT] No data send");
#endif
		}
	}
}
