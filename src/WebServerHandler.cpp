#include "WebServerHandler.h"
#include "Configuration.h"
#include "Credentials.h"
#include <LittleFS.h>
#include <Preferences.h>
extern Preferences preferences;

WebServerHandler::WebServerHandler() 
    : server(80) {}

void WebServerHandler::start() {
    server.serveStatic("/", LittleFS, "/");
    server.on("/", HTTP_GET, handle_index);
    server.on("/data", HTTP_GET, [this](AsyncWebServerRequest* request) { handle_data(request); });
    server.on("/status", HTTP_GET, [this](AsyncWebServerRequest* request) { handle_status(request); });
    server.on("/ap", HTTP_GET, [this](AsyncWebServerRequest* request) { handle_ap(request); });
    server.on("/credentials", HTTP_POST, [this](AsyncWebServerRequest* request) { handle_credentials_submit(request); });
    server.onNotFound(handle_NotFound);
    server.begin();
}

void WebServerHandler::setInputDataforBody(DataCO2 co2Sensordata, Bsec enviromentdata, String sdate)
{
  this->bmedata = enviromentdata;
  this->co2data = co2Sensordata;
  acDate = sdate;
}

// --------------------
// Handlers
// --------------------
void WebServerHandler::handle_index(AsyncWebServerRequest* request) {
  File file = LittleFS.open("/index.html", "r");
  if (!file || file.isDirectory()) {
      request->send(500, "text/plain", "Internal Server Error: Cannot open index.html");
      return;
  }
  String content = file.readString();
  file.close();
  content.replace("{{deviceName}}", DeviceName);
  request->send(200, "text/html; charset=utf-8", content);
}

void WebServerHandler::handle_data(AsyncWebServerRequest* request) {
  String header_data = String("{\n") +
  "\"bme680/temperature\":\"" + String(bmedata.temperature) + "\",\n" +
  "\"bme680/temperature_offset\":\"" + String(bmedata.temperature + TEMPERATUR_OFFSET) + "\",\n" +
  "\"bme680/temperature_raw\":\"" + String(bmedata.rawTemperature) + "\",\n" +
  "\"bme680/humidity\":\"" + String(bmedata.humidity) + "\",\n" +
  "\"bme680/humidity_raw\":\"" + String(bmedata.rawHumidity) + "\",\n" +
  "\"bme680/pressure\":\"" + String(bmedata.pressure) + "\",\n" +
  "\"bme680/gas\":\"" + String(bmedata.gasResistance) + "\",\n" +
  "\"bme680/bme68xStatus\":\"" + String(bmedata.bme68xStatus) + "\",\n" +
  "\"bme680/breathVocAccuracy\":\"" + String(bmedata.breathVocAccuracy) + "\",\n" +
  "\"bme680/breathVocEquivalent\":\"" + String(bmedata.breathVocEquivalent) + "\",\n" +
  "\"bme680/bsecStatus\":\"" + String(bmedata.bsecStatus) + "\",\n" +
  "\"bme680/co2Accuracy\":\"" + String(bmedata.co2Accuracy) + "\",\n" +
  "\"bme680/co2Equivalent\":\"" + String(bmedata.co2Equivalent) + "\",\n" +
  "\"bme680/compGasAccuracy\":\"" + String(bmedata.compGasAccuracy) + "\",\n" +
  "\"bme680/compGasValue\":\"" + String(bmedata.compGasValue) + "\",\n" +
  "\"bme680/gasPercentage\":\"" + String(bmedata.gasPercentage) + "\",\n" +
  "\"bme680/gasPercentageAccuracy\":\"" + String(bmedata.gasPercentageAccuracy) + "\",\n" +
  "\"bme680/iaq\":\"" + String(bmedata.iaq) + "\",\n" +
  "\"bme680/iaqAccuracy\":\"" + String(bmedata.iaqAccuracy) + "\",\n" +
  "\"bme680/staticIaqAccuracy\":\"" + String(bmedata.staticIaqAccuracy) + "\",\n" +
  "\"mhz19/Accuracy\":\"" + String(co2data.getAccuracy()) + "\",\n" +
  "\"mhz19/Background\":\"" + String(co2data.getBackground()) + "\",\n" +
  "\"mhz19/Limited\":\"" + String(co2data.getLimited()) + "\",\n" +
  "\"mhz19/Raw\":\"" + String(co2data.getRaw()) + "\",\n" +
  "\"mhz19/Regular\":\"" + String(co2data.getRegular()) + "\",\n" +
  "\"mhz19/TempAdjustment\":\"" + String(co2data.getTempAdjustment()) + "\",\n" +
  "\"mhz19/Temperature\":\"" + String(co2data.getTemperature()) + "\"\n"
  "}";
  request->send(200, "application/json; charset=utf-8", header_data);
}

void WebServerHandler::handle_status(AsyncWebServerRequest* request) {
  File file = LittleFS.open("/status.html", "r");
  if (!file || file.isDirectory()) {
      request->send(500, "text/plain", "Internal Server Error: Cannot open status.html");
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

void WebServerHandler::handle_ap(AsyncWebServerRequest* request) {
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

  File file = LittleFS.open("/ap.html", "r");
  if (!file) {
    request->send(500, "text/plain", "Failed to open HTML file.");
    return;
  }
  String content = file.readString();
  file.close();

  content.replace("{{ssid}}", String(ssid));
  content.replace("{{password}}", String(password));

  request->send(200, "text/html", content);
}

void WebServerHandler::handle_credentials_submit(AsyncWebServerRequest* request) {
    // Extract SSID and Password from form submission
    String ssid = request->getParam("ssid", true)->value();
    String password = request->getParam("password", true)->value();
  
    // Save credentials to flash memory
    preferences.begin("wifi", false); // Open preferences in read-write mode
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    preferences.end();
  
    // Restart ESP32 to apply new WiFi credentials
    ESP.restart();
}

void WebServerHandler::handle_NotFound(AsyncWebServerRequest* request) {
  request->send(404, "text/plain", "404: Not found");
}



// --------------------
// Helpers
// --------------------
void WebServerHandler::replaceColorDescr(String& str, const String& key, const String& color, const String& descr) {
  str.replace("{{color_" + key + "}}", color);
  str.replace("{{descr_" + key + "}}", descr);
}

void WebServerHandler::replaceIaqAccuracy(String& str, int iaqAccuracy) {
  str.replace("{{data_iaqaccuracy}}", String(iaqAccuracy));

  switch (iaqAccuracy) {
      case 0: replaceColorDescr(str, "iaqaccuracy", "#777", "Calibration phase. Please wait...."); break;
      case 1: replaceColorDescr(str, "iaqaccuracy", "#FFFF00", "learning"); break;
      case 2: replaceColorDescr(str, "iaqaccuracy", "#ADFF2F", "good"); break;
      case 3: replaceColorDescr(str, "iaqaccuracy", "#006B3C", "good. start saving them."); break;
      default: replaceColorDescr(str, "iaqaccuracy", "#FF0000", "this should never happen! iaqAccuracy < 0 || iaqAccuracy > 3"); break;
  }
}

void WebServerHandler::replaceTemperatureInfo(String& str, float temp) {
  if (temp < 16) replaceColorDescr(str, "temp", "#add8e6", "too cold");
  else if (temp < 18) replaceColorDescr(str, "temp", "#0000ff", "cold");
  else if (temp < 20) replaceColorDescr(str, "temp", "#2e8b57", "cool");
  else if (temp < 22) replaceColorDescr(str, "temp", "#00ff00", "normal");
  else if (temp < 24) replaceColorDescr(str, "temp", "#adff2f", "cosy");
  else if (temp < 26) replaceColorDescr(str, "temp", "#ffff00", "warm");
  else if (temp < 28) replaceColorDescr(str, "temp", "#ffa500", "hot");
  else replaceColorDescr(str, "temp", "#ff0000", "scalding hot");
}

void WebServerHandler::replaceHumidityInfo(String& str, float hum) {
  if (hum < 20) replaceColorDescr(str, "relativehumidity", "#ff0000", "Far too dry");
  else if (hum < 30) replaceColorDescr(str, "relativehumidity", "#ffff00", "too dry");
  else if (hum < 40) replaceColorDescr(str, "relativehumidity", "#adff2f", "dry");
  else if (hum < 50) replaceColorDescr(str, "relativehumidity", "#00ff00", "normal");
  else if (hum < 60) replaceColorDescr(str, "relativehumidity", "#adff2f", "Slightly moist");
  else if (hum < 65) replaceColorDescr(str, "relativehumidity", "#ffa500", "moist");
  else if (hum < 70) replaceColorDescr(str, "relativehumidity", "#ff0000", "very moist");
  else replaceColorDescr(str, "relativehumidity", "#ff00ff", "wet");
}

void WebServerHandler::replaceIAQInfo(String& str, float iaq) {
  if (iaq <= 50) replaceColorDescr(str, "iaq", "#2e8b57", "excellent");
  else if (iaq <= 100) replaceColorDescr(str, "iaq", "#00ff00", "good");
  else if (iaq <= 150) replaceColorDescr(str, "iaq", "#9acd32", "lightly polluted. Ventilation suggested.");
  else if (iaq <= 200) replaceColorDescr(str, "iaq", "#ffff00", "moderately polluted. please ventilate.");
  else if (iaq < 250) replaceColorDescr(str, "iaq", "#ffa500", "heavily polluted. please ventilate.");
  else if (iaq < 300) replaceColorDescr(str, "iaq", "#ff0000", "severely polluted. please ventilate urgently.");
  else replaceColorDescr(str, "iaq", "#ff00ff", "extremely polluted. please ventilate urgently.");
}

void WebServerHandler::replaceCO2Info(String& str, int co2) {
  if (co2 < 600) replaceColorDescr(str, "MHZ19B_co2", "#0000ff", "outdoor air");
  else if (co2 < 800) replaceColorDescr(str, "MHZ19B_co2", "#00ff00", "fresh indoor air");
  else if (co2 < 1000) replaceColorDescr(str, "MHZ19B_co2", "#adff2f", "Indoor air");
  else if (co2 < 1200) replaceColorDescr(str, "MHZ19B_co2", "#ffff00", "used indoor air. please ventilate");
  else if (co2 < 1400) replaceColorDescr(str, "MHZ19B_co2", "#ffa500", "stale indoor air. please ventilate");
  else if (co2 < 1600) replaceColorDescr(str, "MHZ19B_co2", "#ff0000", "strongly stale indoor air. thinking performance impaired");
  else replaceColorDescr(str, "MHZ19B_co2", "#ff00ff", "Warning. Tiredness, headache. ventilate urgently.");
}