#include "WebServerHandler.h"
#include "Configuration.h"
#include "Credentials.h"
#include <LittleFS.h>
#include "settingsHandler.h"
SettingsHandler settingsHandler;
#include <Preferences.h>
extern Preferences preferences;

WebServerHandler::WebServerHandler()
	: server(80) {}

void WebServerHandler::start()
{
	server.serveStatic("/", LittleFS, "/");
	server.on("/", HTTP_GET, handle_page_index);
	server.on("/json", HTTP_GET, [this](AsyncWebServerRequest *request)
			  { handle_page_data(request); });
	server.on("/status", HTTP_GET, [this](AsyncWebServerRequest *request)
			  { handle_page_status(request); });
	server.on("/sensorsettings", HTTP_GET, [this](AsyncWebServerRequest *request)
			  { handle_page_sensorsettings(request); });
	server.on("/WLAN", HTTP_GET, [this](AsyncWebServerRequest *request)
			  { handle_page_wlan(request); });
	server.on("/submitWLANcredentials", HTTP_POST, [this](AsyncWebServerRequest *request)
			  { handle_submit_WLANcredentials(request); });
	server.on("/submitmodulinterval", HTTP_POST, [this](AsyncWebServerRequest *request)
			  { handle_submit_modulinterval(request); });
	server.on("/submitmodulswitch", HTTP_POST, [this](AsyncWebServerRequest *request)
			  { handle_submit_modulswitch(request); });
	server.on("/restoredefaultconfiguration", HTTP_POST, [this](AsyncWebServerRequest *request)
			  { handle_restoreDefaultSettings(request); });
	server.on("/restart", HTTP_POST, [this](AsyncWebServerRequest *request)
			  { handle_restart(request); });
	server.onNotFound(handle_page_NotFound);
	server.begin();
}

// --------------------
// Handlers
// --------------------
void WebServerHandler::handle_page_index(AsyncWebServerRequest *request)
{
	File file = LittleFS.open("/index.htm", "r");
	if (!file || file.isDirectory())
	{
		request->send(500, "text/plain", "Internal Server Error: Cannot open index.htm");
		return;
	}
	String content_index = file.readString();
	file.close();

	content_index.replace("{{deviceName}}", DeviceName);
	request->send(200, "text/html; charset=utf-8", content_index);
}

void WebServerHandler::handle_page_data(AsyncWebServerRequest *request)
{
	String header_data = "{\n";
	header_data += "\"bme680/temperature\":\"{{temperature}}\",\n";
	header_data += "\"bme680/temperature_offset\":\"{{temperature_offset}}\",\n";
	header_data += "\"bme680/temperature_raw\":\"{{temperature_raw}}\",\n";
	header_data += "\"bme680/humidity\":\"{{humidity}}\",\n";
	header_data += "\"bme680/humidity_raw\":\"{{humidity_raw}}\",\n";
	header_data += "\"bme680/pressure\":\"{{pressure}}\",\n";
	header_data += "\"bme680/gas\":\"{{gas}}\",\n";
	header_data += "\"bme680/bme68xStatus\":\"{{bme68xStatus}}\",\n";
	header_data += "\"bme680/breathVocAccuracy\":\"{{breathVocAccuracy}}\",\n";
	header_data += "\"bme680/breathVocEquivalent\":\"{{breathVocEquivalent}}\",\n";
	header_data += "\"bme680/bsecStatus\":\"{{bsecStatus}}\",\n";
	header_data += "\"bme680/co2Accuracy\":\"{{co2Accuracy}}\",\n";
	header_data += "\"bme680/co2Equivalent\":\"{{co2Equivalent}}\",\n";
	header_data += "\"bme680/compGasAccuracy\":\"{{compGasAccuracy}}\",\n";
	header_data += "\"bme680/compGasValue\":\"{{compGasValue}}\",\n";
	header_data += "\"bme680/gasPercentage\":\"{{gasPercentage}}\",\n";
	header_data += "\"bme680/gasPercentageAccuracy\":\"{{gasPercentageAccuracy}}\",\n";
	header_data += "\"bme680/iaq\":\"{{iaq}}\",\n";
	header_data += "\"bme680/iaqAccuracy\":\"{{iaqAccuracy}}\",\n";
	header_data += "\"bme680/staticIaqAccuracy\":\"{{staticIaqAccuracy}}\",\n";
	header_data += "\"mhz19/Accuracy\":\"{{mhz19Accuracy}}\",\n";
	header_data += "\"mhz19/Background\":\"{{mhz19Background}}\",\n";
	header_data += "\"mhz19/Limited\":\"{{mhz19Limited}}\",\n";
	header_data += "\"mhz19/Raw\":\"{{mhz19Raw}}\",\n";
	header_data += "\"mhz19/Regular\":\"{{mhz19Regular}}\",\n";
	header_data += "\"mhz19/TempAdjustment\":\"{{mhz19TempAdjustment}}\",\n";
	header_data += "\"mhz19/Temperature\":\"{{mhz19Temperature}}\"\n";
	header_data += "}";

	header_data.replace("{{temperature}}", String(bmedata.temperature));
	header_data.replace("{{temperature_offset}}", String(bmedata.temperature + TEMPERATUR_OFFSET));
	header_data.replace("{{temperature_raw}}", String(bmedata.rawTemperature));
	header_data.replace("{{humidity}}", String(bmedata.humidity));
	header_data.replace("{{humidity_raw}}", String(bmedata.rawHumidity));
	header_data.replace("{{pressure}}", String(bmedata.pressure));
	header_data.replace("{{gas}}", String(bmedata.gasResistance));
	header_data.replace("{{bme68xStatus}}", String(bmedata.bme68xStatus));
	header_data.replace("{{breathVocAccuracy}}", String(bmedata.breathVocAccuracy));
	header_data.replace("{{breathVocEquivalent}}", String(bmedata.breathVocEquivalent));
	header_data.replace("{{bsecStatus}}", String(bmedata.bsecStatus));
	header_data.replace("{{co2Accuracy}}", String(bmedata.co2Accuracy));
	header_data.replace("{{co2Equivalent}}", String(bmedata.co2Equivalent));
	header_data.replace("{{compGasAccuracy}}", String(bmedata.compGasAccuracy));
	header_data.replace("{{compGasValue}}", String(bmedata.compGasValue));
	header_data.replace("{{gasPercentage}}", String(bmedata.gasPercentage));
	header_data.replace("{{gasPercentageAccuracy}}", String(bmedata.gasPercentageAccuracy));
	header_data.replace("{{iaq}}", String(bmedata.iaq));
	header_data.replace("{{iaqAccuracy}}", String(bmedata.iaqAccuracy));
	header_data.replace("{{staticIaqAccuracy}}", String(bmedata.staticIaqAccuracy));
	header_data.replace("{{mhz19Accuracy}}", String(co2data.getAccuracy()));
	header_data.replace("{{mhz19Background}}", String(co2data.getBackground()));
	header_data.replace("{{mhz19Limited}}", String(co2data.getLimited()));
	header_data.replace("{{mhz19Raw}}", String(co2data.getRaw()));
	header_data.replace("{{mhz19Regular}}", String(co2data.getRegular()));
	header_data.replace("{{mhz19TempAdjustment}}", String(co2data.getTempAdjustment()));
	header_data.replace("{{mhz19Temperature}}", String(co2data.getTemperature()));

	request->send(200, "application/json; charset=utf-8", header_data);
}

void WebServerHandler::handle_page_status(AsyncWebServerRequest *request)
{
	File file = LittleFS.open("/status.htm", "r");
	if (!file || file.isDirectory())
	{
		request->send(500, "text/plain", "Internal Server Error: Cannot open status.htm");
		return;
	}
	String header_data = file.readString();
	file.close();
	header_data.replace("{{deviceName}}", DeviceName);
	header_data.replace("{{data_gas}}", String(bmedata.gasPercentage));
	header_data.replace("{{data_breahtvoc}}", String(bmedata.breathVocEquivalent));
	header_data.replace("{{data_pressure}}", String(bmedata.pressure));
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

	preferences.begin("wifi", true);
	ssid = preferences.getString("ssid", "");
	password = preferences.getString("password", "");
	preferences.end();

	if (ssid == "" || password == "")
	{
		ssid = WIFI_SSID;
		password = WIFI_PW;
	}

	File file = LittleFS.open("/wlan.htm", "r");
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

void WebServerHandler::handle_page_sensorsettings(AsyncWebServerRequest *request)
{
	File file = LittleFS.open("/setting.htm", "r");
	if (!file || file.isDirectory())
	{
		request->send(500, "text/plain", "Internal Server Error: Cannot open setting.htm");
		return;
	}
	String content = file.readString();
	file.close();
	content.replace("{{deviceName}}", DeviceName);

	content.replace("{{intervalMHZ19}}", String(settingsHandler.getConfigModul("intervalMHZ19")));
	content.replace("{{intervalBME680}}", String(settingsHandler.getConfigModul("intervalBME680")));
	content.replace("{{intervalWiFi}}", String(settingsHandler.getConfigModul("intervalWiFi")));
	content.replace("{{intervalPRINT}}", String(settingsHandler.getConfigModul("intervalPRINT")));
	content.replace("{{intervalEPD}}", String(settingsHandler.getConfigModul("intervalEPD")));
	content.replace("{{intervalLED}}", String(settingsHandler.getConfigModul("intervalLED")));
	content.replace("{{intervalMQTT}}", String(settingsHandler.getConfigModul("intervalMQTT")));

	content.replace("{{switchWIFI_checked}}", settingsHandler.getConfigModul("switchWIFI") ? "checked" : "");
	content.replace("{{switchEPD_checked}}", settingsHandler.getConfigModul("switchEPD") ? "checked" : "");
	content.replace("{{switchLED_checked}}", settingsHandler.getConfigModul("switchLED") ? "checked" : "");
	content.replace("{{switchMQTT_checked}}", settingsHandler.getConfigModul("switchMQTT") ? "checked" : "");

	request->send(200, "text/html; charset=utf-8", content);
}

void WebServerHandler::handle_submit_WLANcredentials(AsyncWebServerRequest *request)
{
	preferences.begin("config", false);
	preferences.putString("wlanSSID", request->getParam("wlanSSID", true)->value());
	preferences.putString("wlanPASSWORD", request->getParam("wlanPASSWORD", true)->value());
	preferences.end();
	ESP.restart();
}

void WebServerHandler::handle_page_NotFound(AsyncWebServerRequest *request)
{
	request->send(404, "text/plain", "404: Not found");
}

void WebServerHandler::handle_submit_modulinterval(AsyncWebServerRequest *request)
{
	preferences.begin("config", false);

	if (request->hasParam("intervalMHZ19"))
	{
		settingsHandler.setConfigModul("intervalMHZ19", request->getParam("intervalMHZ19")->value().toInt());
	}
	else
	{
		settingsHandler.setConfigModul("intervalMHZ19", interval_MHZ19_in_Seconds);
	}

	if (request->hasParam("intervalBME680"))
	{
		settingsHandler.setConfigModul("intervalBME680", request->getParam("intervalBME680")->value().toInt());
	}
	else
	{
		settingsHandler.setConfigModul("intervalBME680", interval_BME680_in_Seconds);
	}

	if (request->hasParam("intervalWiFi"))
	{
		settingsHandler.setConfigModul("intervalWiFi", request->getParam("intervalWiFi")->value().toInt());
	}
	else
	{
		settingsHandler.setConfigModul("intervalWiFi", interval_WiFiCheck_in_Seconds);
	}

	if (request->hasParam("intervalPRINT"))
	{
		settingsHandler.setConfigModul("intervalPRINT", request->getParam("intervalPRINT")->value().toInt());
	}
	else
	{
		settingsHandler.setConfigModul("intervalPRINT", interval_RAMPrintout_in_Seconds);
	}

	if (request->hasParam("intervalEPD"))
	{
		settingsHandler.setConfigModul("intervalEPD", request->getParam("intervalEPD")->value().toInt());
	}
	else
	{
		settingsHandler.setConfigModul("intervalEPD", interval_EPD_in_Seconds);
	}

	if (request->hasParam("intervalLED"))
	{
		settingsHandler.setConfigModul("intervalLED", request->getParam("intervalLED")->value().toInt());
	}
	else
	{
		settingsHandler.setConfigModul("intervalLED", interval_LED_in_Seconds);
	}

	if (request->hasParam("intervalMQTT"))
	{
		settingsHandler.setConfigModul("intervalMQTT", request->getParam("intervalMQTT")->value().toInt());
	}
	else
	{
		settingsHandler.setConfigModul("intervalMQTT", interval_mqtt_in_Seconds);
	}

	preferences.end();
	request->send(200, "text/plain", "Interval Settings Saved!");
	request->redirect("/");
}

void WebServerHandler::handle_submit_modulswitch(AsyncWebServerRequest *request)
{
	if (request->hasParam("switchWIFI"))
	{
		settingsHandler.setConfigModul("switchWIFI", atoi(request->getParam("switchWIFI")->value().c_str()));
	}
	if (request->hasParam("switchEPD"))
	{
		settingsHandler.setConfigModul("switchEPD", atoi(request->getParam("switchEPD")->value().c_str()));
	}
	if (request->hasParam("switchLED"))
	{
		settingsHandler.setConfigModul("switchLED", atoi(request->getParam("switchLED")->value().c_str()));
	}
	if (request->hasParam("switchMQTT"))
	{
		settingsHandler.setConfigModul("switchMQTT", atoi(request->getParam("switchMQTT")->value().c_str()));
	}
	request->redirect("/");
}

void WebServerHandler::handle_restoreDefaultSettings(AsyncWebServerRequest *request)
{
	settingsHandler.restoreDefaultConfiguration(); // Load default settings
	request->redirect("/");
	request->send(200, "text/plain", "Defaults loaded");
	delay(500);
	ESP.restart();
}

void WebServerHandler::handle_restart(AsyncWebServerRequest *request)
{
	request->send(200, "text/html", "Device is restarting...");
	delay(500);
	ESP.restart();
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