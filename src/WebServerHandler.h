#ifndef CO2_TURTLE_WEBSERVERHANDLER_H
#define CO2_TURTLE_WEBSERVERHANDLER_H

#include "ESPAsyncWebServer.h"
#include "DataCO2.h"
#include "bsec.h"

class WebServerHandler
{
public:
	static WebServerHandler &getInstance()
	{
		static WebServerHandler instance;
		return instance;
	}
	void start();
	void setInputDataforBody(DataCO2 co2Sensordata, Bsec enviromentdata, String sdate);

private:
	DataCO2 co2data;
	Bsec bmedata;
	AsyncWebServer server;
	String acDate;

	WebServerHandler(); // Constructor
	~WebServerHandler() = default;

	// Delete copy constructor and copy assignment to enforce singleton
	WebServerHandler(const WebServerHandler &) = delete;
	WebServerHandler &operator=(const WebServerHandler &) = delete;

	// Request handlers
	static void handle_page_index(AsyncWebServerRequest *request);
	void handle_page_data(AsyncWebServerRequest *request);
	void handle_page_status(AsyncWebServerRequest *request);
	void handle_page_settings(AsyncWebServerRequest *request);
	void handle_page_wlan(AsyncWebServerRequest *request);
	static void handle_page_NotFound(AsyncWebServerRequest *request);
	void handle_submit_WLANcredentials(AsyncWebServerRequest *request);
	void handle_submit_modulinterval(AsyncWebServerRequest *request);
	void handle_submit_modulswitch(AsyncWebServerRequest *request);
	void handle_submit_ledconfig(AsyncWebServerRequest *request);
	void handle_submit_sensorconfig(AsyncWebServerRequest *request);
	void handle_option_restoreDefaultConfiguration(AsyncWebServerRequest *request);
	void handle_option_restart(AsyncWebServerRequest *request);
	void handle_option_calibrate_mhz19(AsyncWebServerRequest *request);


	void handle_page_mqtt(AsyncWebServerRequest *request);
	void handle_submit_mqttconfig(AsyncWebServerRequest *request);
	void handle_api_mqtt_status(AsyncWebServerRequest *request);


	// helper function
	void replaceColorDescr(String &str, const String &key, const String &color, const String &descr);
	void replaceIaqAccuracy(String &str, int iaqAccuracy);
	void replaceTemperatureInfo(String &str, float temp);
	void replaceHumidityInfo(String &str, float hum);
	void replaceIAQInfo(String &str, float iaq);
	void replaceCO2Info(String &str, int co2);
};
#endif // CO2_TURTLE_WEBSERVERHANDLER_H
