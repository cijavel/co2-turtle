#include "WebServerHandler.h"
#include "Configuration.h"
#include "Credentials.h"
#include <LittleFS.h>
#include "MHZ19Handler.h"
#include "ConfigHandler.h"
extern ConfigHandler configHandler;
#include "LEDHandler.h"

WebServerHandler::WebServerHandler()
	: server(80) {}

void WebServerHandler::start()
{
    server.serveStatic("/static", LittleFS, "/static");
    server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {request->redirect("/index"); });
    server.on("/index", HTTP_GET, [this](AsyncWebServerRequest *request) {handle_page_index(request);});
	server.on("/json", HTTP_GET, [this](AsyncWebServerRequest *request) {handle_page_data(request); });
	server.on("/status", HTTP_GET, [this](AsyncWebServerRequest *request) {handle_page_status(request); });
	server.on("/settings", HTTP_GET, [this](AsyncWebServerRequest *request) {handle_page_settings(request); });
	server.on("/WLAN", HTTP_GET, [this](AsyncWebServerRequest *request) {handle_page_wlan(request); });
	server.on("/submitWLANcredentials", HTTP_POST, [this](AsyncWebServerRequest *request) {handle_submit_WLANcredentials(request); });
	server.on("/submitmodulinterval", HTTP_POST, [this](AsyncWebServerRequest *request) {handle_submit_modulinterval(request); });
	server.on("/submitmodulswitch", HTTP_POST, [this](AsyncWebServerRequest *request) {handle_submit_modulswitch(request); });
	server.on("/submitLEDConfig", HTTP_POST, [this](AsyncWebServerRequest *request) {handle_submit_ledconfig(request); });
	server.on("/submitSensorConfig", HTTP_POST, [this](AsyncWebServerRequest *request) { handle_submit_sensorconfig(request); });
	server.on("/restoredefaultconfiguration", HTTP_POST, [this](AsyncWebServerRequest *request) {handle_option_restoreDefaultConfiguration(request); });
	server.on("/restart", HTTP_POST, [this](AsyncWebServerRequest *request) {handle_option_restart(request); });
	server.on("/calibrateMHZ19", HTTP_POST, [this](AsyncWebServerRequest *request) { handle_option_calibrate_mhz19(request); });
	server.onNotFound(handle_page_NotFound);
	server.begin();
}

// --------------------
// Handlers
// --------------------
void WebServerHandler::handle_page_index(AsyncWebServerRequest *request)
{
	File file = LittleFS.open("/static/index.htm", "r");
	if (!file || file.isDirectory())
	{
		request->send(500, "text/plain", "Internal Server Error: Cannot open index.htm");
		return;
	}
	String content_index = file.readString();
	file.close();

	String deviceNameVar = configHandler.getConfigDevice("deviceName");
	content_index.replace("{{deviceName}}", deviceNameVar);
	request->send(200, "text/html; charset=utf-8", content_index);
}

void WebServerHandler::handle_page_data(AsyncWebServerRequest *request)
{
	String headerData = "{\n";
	headerData += "\"bme680/temperature\":\"{{temperature}}\",\n";
	headerData += "\"bme680/temperature_offset\":\"{{temperature_offset}}\",\n";
	headerData += "\"bme680/temperature_raw\":\"{{temperature_raw}}\",\n";
	headerData += "\"bme680/humidity\":\"{{humidity}}\",\n";
	headerData += "\"bme680/humidity_raw\":\"{{humidity_raw}}\",\n";
	headerData += "\"bme680/pressure\":\"{{pressure}}\",\n";
	headerData += "\"bme680/gas\":\"{{gas}}\",\n";
	headerData += "\"bme680/bme68xStatus\":\"{{bme68xStatus}}\",\n";
	headerData += "\"bme680/breathVocAccuracy\":\"{{breathVocAccuracy}}\",\n";
	headerData += "\"bme680/breathVocEquivalent\":\"{{breathVocEquivalent}}\",\n";
	headerData += "\"bme680/bsecStatus\":\"{{bsecStatus}}\",\n";
	headerData += "\"bme680/co2Accuracy\":\"{{co2Accuracy}}\",\n";
	headerData += "\"bme680/co2Equivalent\":\"{{co2Equivalent}}\",\n";
	headerData += "\"bme680/compGasAccuracy\":\"{{compGasAccuracy}}\",\n";
	headerData += "\"bme680/compGasValue\":\"{{compGasValue}}\",\n";
	headerData += "\"bme680/gasPercentage\":\"{{gasPercentage}}\",\n";
	headerData += "\"bme680/gasPercentageAccuracy\":\"{{gasPercentageAccuracy}}\",\n";
	headerData += "\"bme680/iaq\":\"{{iaq}}\",\n";
	headerData += "\"bme680/iaqAccuracy\":\"{{iaqAccuracy}}\",\n";
	headerData += "\"bme680/staticIaqAccuracy\":\"{{staticIaqAccuracy}}\",\n";
	headerData += "\"bme680/runInStatus\":\"{{runInStatus}}\",\n";
	headerData += "\"bme680/stabStatus\":\"{{stabStatus}}\",\n";
	headerData += "\"bme680/nextCall\":\"{{nextCall}}\",\n";
	headerData += "\"bme680/outputTimestamp\":\"{{outputTimestamp}}\",\n";

	headerData += "\"mhz19/Accuracy\":\"{{mhz19Accuracy}}\",\n";
	headerData += "\"mhz19/Background\":\"{{mhz19Background}}\",\n";
	headerData += "\"mhz19/Limited\":\"{{mhz19Limited}}\",\n";
	headerData += "\"mhz19/Raw\":\"{{mhz19Raw}}\",\n";
	headerData += "\"mhz19/Regular\":\"{{mhz19Regular}}\",\n";
	headerData += "\"mhz19/TempAdjustment\":\"{{mhz19TempAdjustment}}\",\n";
	headerData += "\"mhz19/Temperature\":\"{{mhz19Temperature}}\"\n";
	headerData += "}";

	headerData.replace("{{runInStatus}}", String(bmedata.runInStatus));
	headerData.replace("{{stabStatus}}", String(bmedata.stabStatus));
	headerData.replace("{{nextCall}}", String(bmedata.nextCall));
	headerData.replace("{{outputTimestamp}}", String(bmedata.outputTimestamp));
	headerData.replace("{{temperature}}", String(bmedata.temperature));
	headerData.replace("{{temperature_offset}}", String(bmedata.temperature + configHandler.getConfigSensor("tempOffset") / 10.0f));
	headerData.replace("{{temperature_raw}}", String(bmedata.rawTemperature));
	headerData.replace("{{humidity}}", String(bmedata.humidity));
	headerData.replace("{{humidity_raw}}", String(bmedata.rawHumidity));
	headerData.replace("{{pressure}}", String(bmedata.pressure));
	headerData.replace("{{gas}}", String(bmedata.gasResistance));
	headerData.replace("{{bme68xStatus}}", String(bmedata.bme68xStatus));
	headerData.replace("{{breathVocAccuracy}}", String(bmedata.breathVocAccuracy));
	headerData.replace("{{breathVocEquivalent}}", String(bmedata.breathVocEquivalent));
	headerData.replace("{{bsecStatus}}", String(bmedata.bsecStatus));
	headerData.replace("{{co2Accuracy}}", String(bmedata.co2Accuracy));
	headerData.replace("{{co2Equivalent}}", String(bmedata.co2Equivalent));
	headerData.replace("{{compGasAccuracy}}", String(bmedata.compGasAccuracy));
	headerData.replace("{{compGasValue}}", String(bmedata.compGasValue));
	headerData.replace("{{gasPercentage}}", String(bmedata.gasPercentage));
	headerData.replace("{{gasPercentageAccuracy}}", String(bmedata.gasPercentageAccuracy));
	headerData.replace("{{iaq}}", String(bmedata.iaq));
	headerData.replace("{{iaqAccuracy}}", String(bmedata.iaqAccuracy));
	headerData.replace("{{staticIaqAccuracy}}", String(bmedata.staticIaqAccuracy));
	headerData.replace("{{mhz19Accuracy}}", String(co2data.getAccuracy()));
	headerData.replace("{{mhz19Background}}", String(co2data.getBackground()));
	headerData.replace("{{mhz19Limited}}", String(co2data.getLimited()));
	headerData.replace("{{mhz19Raw}}", String(co2data.getRaw()));
	headerData.replace("{{mhz19Regular}}", String(co2data.getRegular()));
	headerData.replace("{{mhz19TempAdjustment}}", String(co2data.getTempAdjustment()));
	headerData.replace("{{mhz19Temperature}}", String(co2data.getTemperature()));

	request->send(200, "application/json; charset=utf-8", headerData);
}

void WebServerHandler::handle_page_status(AsyncWebServerRequest *request)
{
	File file = LittleFS.open("/static/status.htm", "r");
	if (!file || file.isDirectory())
	{
		request->send(500, "text/plain", "Internal Server Error: Cannot open status.htm");
		return;
	}
	String header_data = file.readString();
	file.close();
	header_data.replace("{{deviceName}}", DeviceName);
	header_data.replace("{{data_gas}}", String(bmedata.gasResistance));
	header_data.replace("{{data_breahtvoc}}", String(bmedata.breathVocEquivalent));
	header_data.replace("{{data_pressure}}", String(bmedata.pressure/100));
	header_data.replace("{{data_timestep}}", String(bmedata.outputTimestamp));
	header_data.replace("{{data_zone}}", TIMEZONE);
	header_data.replace("{{data_time}}", acDate);

	// Sensor Accuracy
	header_data.replace("{{data_iaqaccuracy}}", String(bmedata.iaqAccuracy));
	replaceIaqAccuracy(header_data, bmedata.iaqAccuracy);

	// Temperature
	header_data.replace("{{data_temp}}", String(bmedata.temperature));
	replaceTemperatureInfo(header_data, bmedata.temperature);

	// Humidity
	header_data.replace("{{data_relativehumidity}}", String(bmedata.humidity));
	replaceHumidityInfo(header_data, bmedata.humidity);

	// Air Quality
	header_data.replace("{{data_iaq}}", String(bmedata.iaq));
	replaceIAQInfo(header_data, bmedata.iaq);

	// CO2 Level
	header_data.replace("{{data_MHZ19B_co2}}", String(co2data.getRegular()));
	replaceCO2Info(header_data, co2data.getRegular());

	request->send(200, "text/html; charset=utf-8", header_data);
}

void WebServerHandler::handle_page_wlan(AsyncWebServerRequest *request)
{
	String password;
	String ssid;

	ssid = configHandler.getConfigDevice("wlanSSID");
	password = configHandler.getConfigDevice("wlanPASSWORD");

	if (ssid == "" || password == "")
	{
		ssid = WIFI_SSID;
		password = WIFI_PW;
	}

	File file = LittleFS.open("/static/wlan.htm", "r");
	if (!file)
	{
		request->send(500, "text/plain", "Internal Server Error: Cannot open wlan.htm");
		return;
	}
	String content = file.readString();
	file.close();

	content.replace("{{deviceName}}", DeviceName);
	content.replace("{{ssid}}", String(ssid));
	content.replace("{{password}}", String(password));

	request->send(200, "text/html", content);
}

void WebServerHandler::handle_page_settings(AsyncWebServerRequest *request)
{
	File file = LittleFS.open("/static/settings.htm", "r");
	if (!file || file.isDirectory())
	{
		request->send(500, "text/plain", "Internal Server Error: Cannot open settings.htm");
		return;
	}
	String content = file.readString();
	file.close();
	content.replace("{{deviceName}}", DeviceName);

	content.replace("{{intervalMHZ19}}", String(configHandler.getConfigInterval("intervalMHZ19")));
	content.replace("{{intervalBME680}}", String(configHandler.getConfigInterval("intervalBME680")));
	content.replace("{{intervalWiFi}}", String(configHandler.getConfigInterval("intervalWiFi")));
	content.replace("{{intervalPRINT}}", String(configHandler.getConfigInterval("intervalPRINT")));
	content.replace("{{intervalEPD}}", String(configHandler.getConfigInterval("intervalEPD")));
	content.replace("{{intervalLED}}", String(configHandler.getConfigInterval("intervalLED")));
	content.replace("{{intervalMQTT}}", String(configHandler.getConfigInterval("intervalMQTT")));

	content.replace("{{switchWIFI_checked}}", configHandler.getConfigSwitch("switchWIFI") ? "checked" : "");
	content.replace("{{switchEPD_checked}}", configHandler.getConfigSwitch("switchEPD") ? "checked" : "");
	content.replace("{{switchLED_checked}}", configHandler.getConfigSwitch("switchLED") ? "checked" : "");
	content.replace("{{switchMQTT_checked}}", configHandler.getConfigSwitch("switchMQTT") ? "checked" : "");

	content.replace("{{LEDbrightness}}", String(configHandler.getConfigLED("LEDbrightness")));
	content.replace("{{SEALEVELPRESSURE_HPA}}", String(configHandler.getConfigSensor("pressure")));
	content.replace("{{TEMPERATUR_OFFSET}}", String(configHandler.getConfigSensor("tempOffset")));

	request->send(200, "text/html; charset=utf-8", content);
}

void WebServerHandler::handle_page_NotFound(AsyncWebServerRequest *request)
{
	request->send(404, "text/plain", "404: Not found");
}


void WebServerHandler::handle_submit_WLANcredentials(AsyncWebServerRequest *request)
{
	if (request->hasParam("wlanSSID", true) && request->hasParam("wlanPASSWORD", true))
	{
		configHandler.setConfigDevice("wlanSSID", request->getParam("wlanSSID", true)->value());
		configHandler.setConfigDevice("wlanPASSWORD", request->getParam("wlanPASSWORD", true)->value());
	}
	else
	{
		request->send(400, "text/plain", "Bad Request: Missing parameters");
		return;
	}
	request->send(200, "text/plain", "WLAN Settings Saved!");
	request->redirect("/index");
	ESP.restart();
}

void WebServerHandler::handle_submit_modulinterval(AsyncWebServerRequest *request)
{

	if (request->hasParam("intervalMHZ19"))
	{
		configHandler.setConfigInterval("intervalMHZ19", request->getParam("intervalMHZ19")->value().toInt());
	}
	else
	{
		Serial.println("MHZ19 Interval not set. Using default value.");
		configHandler.setConfigInterval("intervalMHZ19", interval_MHZ19_in_Seconds);
	}

	if (request->hasParam("intervalBME680"))
	{
		configHandler.setConfigInterval("intervalBME680", request->getParam("intervalBME680")->value().toInt());
	}
	else
	{
		Serial.println("BME680 Interval not set. Using default value.");
		configHandler.setConfigInterval("intervalBME680", interval_BME680_in_Seconds);
	}

	if (request->hasParam("intervalWiFi"))
	{
		configHandler.setConfigInterval("intervalWiFi", request->getParam("intervalWiFi")->value().toInt());
	}
	else
	{	
		Serial.println("WiFi Interval not set. Using default value.");
		configHandler.setConfigInterval("intervalWiFi", interval_WiFiCheck_in_Seconds);
	}

	if (request->hasParam("intervalPRINT"))
	{
		configHandler.setConfigInterval("intervalPRINT", request->getParam("intervalPRINT")->value().toInt());
	}
	else
	{
		Serial.println("RAM Printout Interval not set. Using default value.");
		configHandler.setConfigInterval("intervalPRINT", interval_RAMPrintout_in_Seconds);
	}

	if (request->hasParam("intervalEPD"))
	{
		configHandler.setConfigInterval("intervalEPD", request->getParam("intervalEPD")->value().toInt());
	}
	else
	{
		Serial.println("EPD Interval not set. Using default value.");
		configHandler.setConfigInterval("intervalEPD", interval_EPD_in_Seconds);
	}

	if (request->hasParam("intervalLED"))
	{
		configHandler.setConfigInterval("intervalLED", request->getParam("intervalLED")->value().toInt());
	}
	else
	{
		Serial.println("LED Interval not set. Using default value.");
		configHandler.setConfigInterval("intervalLED", interval_LED_in_Seconds);
	}

	if (request->hasParam("intervalMQTT"))
	{

		configHandler.setConfigInterval("intervalMQTT", request->getParam("intervalMQTT")->value().toInt());
	}
	else
	{
		Serial.println("MQTT Interval not set. Using default value.");
		configHandler.setConfigInterval("intervalMQTT", interval_mqtt_in_Seconds);
	}
	request->send(200, "text/plain", "Interval Settings Saved!");
	request->redirect("/settings");
}

void WebServerHandler::handle_submit_modulswitch(AsyncWebServerRequest *request)
{
	if (request->hasParam("switchWIFI"))
	{
		configHandler.setConfigSwitch("switchWIFI", atoi(request->getParam("switchWIFI")->value().c_str()));
	}
	if (request->hasParam("switchEPD"))
	{
		configHandler.setConfigSwitch("switchEPD", atoi(request->getParam("switchEPD")->value().c_str()));
	}
	if (request->hasParam("switchLED"))
	{
		configHandler.setConfigSwitch("switchLED", atoi(request->getParam("switchLED")->value().c_str()));
	}
	if (request->hasParam("switchMQTT"))
	{
		configHandler.setConfigSwitch("switchMQTT", atoi(request->getParam("switchMQTT")->value().c_str()));
	} 
	request->send(200, "text/plain", "Modul Settings Saved!");
	request->redirect("/settings");
}

void WebServerHandler::handle_submit_ledconfig(AsyncWebServerRequest *request)
{
    if (request->hasParam("LEDbrightness", true)) // Prüft POST-Parameter
    {
        int brightness = request->getParam("LEDbrightness", true)->value().toInt();
        configHandler.setConfigLED("LEDbrightness", brightness);

        LEDHandler &ledhandler = LEDHandler::getInstance();
        ledhandler.updateLEDBrightness(brightness);
    }
    else
    {
        request->send(400, "text/plain", "Bad Request: Missing parameters");
        return;
    }
    request->send(200, "text/plain", "LED Settings Saved!");
    request->redirect("/settings");
}

void WebServerHandler::handle_submit_sensorconfig(AsyncWebServerRequest *request)
{
    bool updated = false;

    if (request->hasParam("SEALEVELPRESSURE_HPA", true)) {
        int sealevel = request->getParam("SEALEVELPRESSURE_HPA", true)->value().toInt();
        configHandler.setConfigSensor("pressure", sealevel);
        updated = true;
    }
    if (request->hasParam("TEMPERATUR_OFFSET", true)) {
        float tempOffset = request->getParam("TEMPERATUR_OFFSET", true)->value().toFloat();
        configHandler.setConfigSensor("tempOffset", (int)(tempOffset * 10));
        updated = true;
    }

    if (updated) {
        request->send(200, "text/plain", "Sensor configuration saved!");
    } else {
        request->send(400, "text/plain", "Bad Request: Missing parameters");
    }
}


void WebServerHandler::handle_option_restoreDefaultConfiguration(AsyncWebServerRequest *request)
{
	request->send(200, "text/html",
    "<html><head>"
    "<meta http-equiv='refresh' content='6;url=/'>"
    "</head><body style='font-family:sans-serif;padding:40px'>"
    "<h2>Standardkonfiguration geladen.</h2>"
    "<p>Weiterleitung in 6 Sekunden.</p>"
    "</body></html>");

	xTaskCreate([](void*) {
		vTaskDelay(1500 / portTICK_PERIOD_MS);
		ESP.restart();
	}, "restore_task", 1024, nullptr, 1, nullptr);
}

void WebServerHandler::handle_option_restart(AsyncWebServerRequest *request)
{
	    request->send(200, "text/html",
        "<html><head>"
        "<meta http-equiv='refresh' content='6;url=/'>"
        "</head><body style='font-family:sans-serif;padding:40px'>"
        "<h2>Neustart läuft...</h2>"
        "<p>Weiterleitung in 6 Sekunden.</p>"
        "</body></html>");

    xTaskCreate([](void*) {
        vTaskDelay(1500 / portTICK_PERIOD_MS);
        ESP.restart();
    }, "restart_task", 1024, nullptr, 1, nullptr);
}

void WebServerHandler::handle_option_calibrate_mhz19(AsyncWebServerRequest *request)
{
    MHZ19Handler &mhz19Handler = MHZ19Handler::getInstance();
    mhz19Handler.calibrate();
    request->send(200, "text/plain", "Calibration started.");
	delay(1000);
	request->redirect("/settings");
}

// --------------------
// Helpers
// --------------------
void WebServerHandler::setInputDataforBody(DataCO2 co2Sensordata, Bsec enviromentdata, String sdate)
{
	this->bmedata = enviromentdata;
	this->co2data = co2Sensordata;
	acDate = sdate;
}

void WebServerHandler::replaceColorDescr(String &str, const String &key, const String &color, const String &descr)
{
	str.replace("{{color_" + key + "}}", color);
	str.replace("{{descr_" + key + "}}", descr);
}

void WebServerHandler::replaceIaqAccuracy(String &str, int iaqAccuracy)
{
	str.replace("{{data_iaqaccuracy}}", String(iaqAccuracy));

	switch (iaqAccuracy)
	{
	case 0:
		replaceColorDescr(str, "iaqaccuracy", "#777", "Calibration phase. Please wait....");
		break;
	case 1:
		replaceColorDescr(str, "iaqaccuracy", "#FFFF00", "learning");
		break;
	case 2:
		replaceColorDescr(str, "iaqaccuracy", "#ADFF2F", "good");
		break;
	case 3:
		replaceColorDescr(str, "iaqaccuracy", "#006B3C", "good. start saving them.");
		break;
	default:
		replaceColorDescr(str, "iaqaccuracy", "#FF0000", "this should never happen! iaqAccuracy < 0 || iaqAccuracy > 3");
		break;
	}
}

void WebServerHandler::replaceTemperatureInfo(String &str, float temp)
{
	if (temp < 16)
		replaceColorDescr(str, "temp", "#add8e6", "too cold");
	else if (temp < 18)
		replaceColorDescr(str, "temp", "#0000ff", "cold");
	else if (temp < 20)
		replaceColorDescr(str, "temp", "#2e8b57", "cool");
	else if (temp < 22)
		replaceColorDescr(str, "temp", "#00ff00", "normal");
	else if (temp < 24)
		replaceColorDescr(str, "temp", "#adff2f", "cosy");
	else if (temp < 26)
		replaceColorDescr(str, "temp", "#ffff00", "warm");
	else if (temp < 28)
		replaceColorDescr(str, "temp", "#ffa500", "hot");
	else
		replaceColorDescr(str, "temp", "#ff0000", "scalding hot");
}

void WebServerHandler::replaceHumidityInfo(String &str, float hum)
{
	if (hum < 20)
		replaceColorDescr(str, "relativehumidity", "#ff0000", "Far too dry");
	else if (hum < 30)
		replaceColorDescr(str, "relativehumidity", "#ffff00", "too dry");
	else if (hum < 40)
		replaceColorDescr(str, "relativehumidity", "#adff2f", "dry");
	else if (hum < 50)
		replaceColorDescr(str, "relativehumidity", "#00ff00", "normal");
	else if (hum < 60)
		replaceColorDescr(str, "relativehumidity", "#adff2f", "Slightly moist");
	else if (hum < 65)
		replaceColorDescr(str, "relativehumidity", "#ffa500", "moist");
	else if (hum < 70)
		replaceColorDescr(str, "relativehumidity", "#ff0000", "very moist");
	else
		replaceColorDescr(str, "relativehumidity", "#ff00ff", "wet");
}

void WebServerHandler::replaceIAQInfo(String &str, float iaq)
{
	if (iaq <= 50)
		replaceColorDescr(str, "iaq", "#2e8b57", "excellent");
	else if (iaq <= 100)
		replaceColorDescr(str, "iaq", "#00ff00", "good");
	else if (iaq <= 150)
		replaceColorDescr(str, "iaq", "#9acd32", "lightly polluted. Ventilation suggested.");
	else if (iaq <= 200)
		replaceColorDescr(str, "iaq", "#ffff00", "moderately polluted. please ventilate.");
	else if (iaq < 250)
		replaceColorDescr(str, "iaq", "#ffa500", "heavily polluted. please ventilate.");
	else if (iaq < 300)
		replaceColorDescr(str, "iaq", "#ff0000", "severely polluted. please ventilate urgently.");
	else
		replaceColorDescr(str, "iaq", "#ff00ff", "extremely polluted. please ventilate urgently.");
}

void WebServerHandler::replaceCO2Info(String &str, int co2)
{
	if (co2 < 600)
		replaceColorDescr(str, "MHZ19B_co2", "#0000ff", "outdoor air");
	else if (co2 < 800)
		replaceColorDescr(str, "MHZ19B_co2", "#00ff00", "fresh indoor air");
	else if (co2 < 1000)
		replaceColorDescr(str, "MHZ19B_co2", "#adff2f", "Indoor air");
	else if (co2 < 1200)
		replaceColorDescr(str, "MHZ19B_co2", "#ffff00", "used indoor air. please ventilate");
	else if (co2 < 1400)
		replaceColorDescr(str, "MHZ19B_co2", "#ffa500", "stale indoor air. please ventilate");
	else if (co2 < 1600)
		replaceColorDescr(str, "MHZ19B_co2", "#ff0000", "strongly stale indoor air. thinking performance impaired");
	else
		replaceColorDescr(str, "MHZ19B_co2", "#ff00ff", "Warning. Tiredness, headache. ventilate urgently.");
}