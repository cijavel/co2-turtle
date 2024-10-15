#include "WebServerHandler.h"
#include "Configuration.h"
#include "Credentials.h"
#include <Preferences.h>
Preferences preferences;

const String unifiedCSS = R"=====(
  <style>
    body {
      font-family: Arial, Helvetica, sans-serif;
    }
    h1 {
      font-size: 120%;
    }
    table.content {
      width: 100%;
      border-collapse: collapse;
      margin-top: 20px;
      font-family: Arial, Helvetica, sans-serif;
    }
    td.description {
      font-size: 14px;
      color: #666;
    }
    th.title {
      font-weight: bold;
      text-align: left;
      padding: 8px;
    }
    td.data {
      text-align: left;
      padding: 8px;
      width: 40px;
    }
    td.space {
      width: 20px;
    }
    .status {
      border-radius: 50%;
      width: 20px;
      height: 20px;
    }
    .button {
      padding: 10px 20px;
      margin: 5px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: 16px;
      color: white;
      background-color: #4CAF50;
      border: none;
      border-radius: 4px;
      cursor: pointer;
    }
    .button:hover {
      background-color: #45a049;
    }
    .input {
      padding: 10px;
      margin: 5px;
      font-size: 16px;
      border-radius: 4px;
      border: 1px solid #ccc;
    }
    .status {
      border-radius: 50%;
      padding: 0px 10px;
    }
    .saved-credentials {
      font-family: Arial, Helvetica, sans-serif;
      margin: 10px 0;
      padding: 10px;
      border: 1px solid #ccc;
      border-radius: 4px;
      background-color: #f9f9f9;
    }
    .show-password-button {
      padding: 5px 10px;
      margin-left: 10px;
      cursor: pointer;
      background-color: #f0f0f0;
      border: 1px solid #ccc;
      border-radius: 4px;
    }
  </style>
)=====";

WebServerHandler::WebServerHandler()
{
  server.on("/", HTTP_GET, handle_index);
  server.on("/json", HTTP_GET, std::bind(&WebServerHandler::handle_data, this, std::placeholders::_1));
  server.on("/status", HTTP_GET, std::bind(&WebServerHandler::handle_status, this, std::placeholders::_1));
  server.on("/WLAN", HTTP_GET, std::bind(&WebServerHandler::handle_ap, this, std::placeholders::_1));
  server.on("/submit", HTTP_POST, std::bind(&WebServerHandler::handle_credentials_submit, this, std::placeholders::_1));

  server.onNotFound(handle_NotFound);
}

void WebServerHandler::handle_index(AsyncWebServerRequest *request)
{
  String content = R"=====(<html>
    <head>
      <title>Index</title>
      <meta name="viewport" content="width=device-width, initial-scale=1">
    )=====" + unifiedCSS + R"=====(</head>
    <body>
    <h1>{{deviceName}}</h1>
      <div>
        <button class="button" onclick="location.href='/status'">Status</button>
        <button class="button" onclick="location.href='/json'">JSON File</button>
        <button class="button" onclick="location.href='/WLAN'">WLAN Settings</button>
      </div>
    </body>
  </html>
  )=====";
  content.replace("{{deviceName}}", DeviceName);
  request->send(200, "text/html; charset=utf-8", content);
}

void WebServerHandler::handle_data(AsyncWebServerRequest *request)
{
  String header_data = '{\n\"bme680/temperature' + '\":\"' + String(bmedata.temperature) + '\",\n' + '\"' +
                       '\"bme680/temperature_offset' + '\":\"' + String(bmedata.temperature + TEMPERATUR_OFFSET) + '\",\n' + '\"' +
                       '\"bme680/temperature_raw' + '\":\"' + String(bmedata.rawTemperature) + '\",\n' + '\"' +
                       '\"bme680/humidity' + '\":\"' + String(bmedata.humidity) + '\",\n' + '\"' +
                       '\"bme680/humidity_raw' + '\":\"' + String(bmedata.rawHumidity) + '\",\n' + '\"' +
                       '\"bme680/pressure' + '\":\"' + String(bmedata.pressure) + '\",\n' + '\"' +
                       '\"bme680/gas' + '\":\"' + String(bmedata.gasResistance) + '\",\n' + '\"' +
                       '\"bme680/bme68xStatus' + '\":\"' + String(bmedata.bme68xStatus) + '\",\n' + '\"' +
                       '\"bme680/breathVocAccuracy' + '\":\"' + String(bmedata.breathVocAccuracy) + '\",\n' + '\"' +
                       '\"bme680/breathVocEquivalent' + '\":\"' + String(bmedata.breathVocEquivalent) + '\",\n' + '\"' +
                       '\"bme680/bsecStatus' + '\":\"' + String(bmedata.bsecStatus) + '\",\n' + '\"' +
                       '\"bme680/co2Accuracy' + '\":\"' + String(bmedata.co2Accuracy) + '\",\n' + '\"' +
                       '\"bme680/co2Equivalent' + '\":\"' + String(bmedata.co2Equivalent) + '\",\n' + '\"' +
                       '\"bme680/compGasAccuracy' + '\":\"' + String(bmedata.compGasAccuracy) + '\",\n' + '\"' +
                       '\"bme680/compGasValue' + '\":\"' + String(bmedata.compGasValue) + '\",\n' + '\"' +
                       '\"bme680/gasPercentage' + '\":\"' + String(bmedata.gasPercentage) + '\",\n' + '\"' +
                       '\"bme680/gasPercentageAccuracy' + '\":\"' + String(bmedata.gasPercentageAccuracy) + '\",\n' + '\"' +
                       '\"bme680/iaq' + '\":\"' + String(bmedata.iaq) + '\",\n' + '\"' +
                       '\"bme680/iaqAccuracy' + '\":\"' + String(bmedata.iaqAccuracy) + '\",\n' + '\"' +
                       '\"bme680/staticIaqAccuracy' + '\":\"' + String(bmedata.staticIaqAccuracy) + '\",\n' + '\"' +
                       '\"mhz19/Accuracy' + '\":\"' + String(co2data.getAccuracy()) + '\",\n' + '\"' +
                       '\"mhz19/Background' + '\":\"' + String(co2data.getBackground()) + '\",\n' + '\"' +
                       '\"mhz19/Limited' + '\":\"' + String(co2data.getLimited()) + '\",\n' + '\"' +
                       '\"mhz19/Raw' + '\":\"' + String(co2data.getRaw()) + '\",\n' + '\"' +
                       '\"mhz19/Regular' + '\":\"' + String(co2data.getRegular()) + '\",\n' + '\"' +
                       '\"mhz19/TempAdjustment' + '\":\"' + String(co2data.getTempAdjustment()) + '\",\n' + '\"' +
                       '\"mhz19/Temperature' + '\":\"' + String(co2data.getTemperature()) + '\"\n}';

  request->send(200, "application/json; charset=utf-8", header_data);
}

void WebServerHandler::handle_ap(AsyncWebServerRequest *request)
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

  String content = R"=====(
    <!DOCTYPE html>
    <html>
    <head>
      <title>WLAN Settings</title>
      )=====" + unifiedCSS + R"=====(
    </head>
    <body>
      <h1>WLAN Settings</h1>
      <br>
      <button class="button" onclick="window.location.href='/'">Back to Index</button>
      <br>
      <div class="saved-credentials">
        <h3>Saved Credentials:</h3>
        <p><strong>SSID:</strong> {{ssid}}</p>
        <p>
          <strong>Password:</strong> 
          <span id="saved-password">********</span>
          <button type="button" class="show-password-button" onmousedown="showSavedPassword()" onmouseup="hideSavedPassword()" onmouseleave="hideSavedPassword()">Show Password</button>
        </p>
      </div>
      <form action="/submit" method="POST">
        SSID: <input class="input" type="text" name="ssid"><br>
        Password: <input class="input" type="password" name="password" id="password"><br>
        <button type="button" class="show-password-button" onmousedown="showPassword()" onmouseup="hidePassword()" onmouseleave="hidePassword()">Show Password</button>
        <br><br>
        <input class="button" type="submit" value="Submit">
      </form>
      <script>
        function showPassword() {
          document.getElementById('password').type = 'text';
        }
        function hidePassword() {
          document.getElementById('password').type = 'password';
        }
        function showSavedPassword() {
          document.getElementById('saved-password').innerText = '{{password}}';
        }
        function hideSavedPassword() {
          document.getElementById('saved-password').innerText = '********';
        }
      </script>
    </body>
    </html>
  )=====";

  content.replace("{{ssid}}", String(ssid));
  content.replace("{{password}}", String(password));

  request->send(200, "text/html", content);
}

void WebServerHandler::handle_credentials_submit(AsyncWebServerRequest *request)
{
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

void WebServerHandler::handle_status(AsyncWebServerRequest *request)
{
  String header_data = R"=====(<!DOCTYPE HTML>
  <html>
    <head>
      )=====" + unifiedCSS +R"=====(
      <title>sensor status</title>
      <meta name="viewport" content="width=device-width, initial-scale=1">
    </head>
    <body>
      <h1>{{deviceName}}</h1>
      <br>
      <button class="button" onclick="window.location.href='/'">Back to Index</button>
      <br>
      <table class="content">
        <tr>
          <th class="title">Sensor Accuracy</th>
          <td class="space"> </td>
          <td class="data" >{{data_iaqaccuracy}} level</td>
          <td class="space"> </td>
          <td class="status" style="background-color: #{{color_iaqaccuracy}};"> </td>
          <td class="space"> </td>
          <td class="description" >{{descr_iaqaccuracy}}</td>
        </tr>
        <tr>
          <th class="title">Temperature</th>
          <td class="space"> </td>
          <td class="data" >{{data_temp}} °C</td>
          <td class="space"> </td>
          <td class="status" style="background-color: #{{color_temp}}; "> </td>
          <td class="space"> </td>
          <td class="data" >{{descr_temp}}</td>
        </tr>
        <tr>
          <th class="title">Humidity</th>
          <td class="space"> </td>
          <td class="data" >{{data_relativehumidity}} %</td>
          <td class="space"> </td>
          <td class="status" style="background-color: #{{color_relativehumidity}};"> </td>
          <td class="space"> </td>
          <td class="data" >{{descr_relativehumidity}}</td>
        </tr>
        <tr>
          <th class="title">Air Quality </th>
          <td class="space"> </td>
          <td class="data" >{{data_iaq}} IAQ</td>
          <td class="space"> </td>
          <td class="status" style="background-color: #{{color_iaq}};"> </td>
          <td class="space"> </td>
          <td class="data" >{{descr_iaq}}</td>
        </tr>
        <tr>
          <th class="title">Co2 Level</th>
          <td class="space"> </td>
          <td class="data" >{{data_MHZ19B_co2}} ppm</td>
          <td class="space"> </td>
          <td class="status" style="background-color: #{{color_MHZ19B_co2}};"> </td>
          <td class="space"> </td>
          <td class="data" >{{descr_MHZ19B_co2}}</td>
        </tr>
        <tr>
          <th class="title"> .....  </th>
        </tr>
         <tr>
          <th class="title">Air pressure</th>
          <td class="space"> </td>
          <td class="data" >{{data_pressure}} hPa</td>
        </tr>
        <tr>
          <th class="title">Gas resistance</th>
          <td class="space"> </td>
          <td class="data" >{{data_gas}} Ohm</td>
        </tr>
        <tr>
          <th class="title">breath VOC Equivalent</th>
          <td class="space"> </td>
          <td class="data" >{{data_breahtvoc}} ppm</td>
        </tr>
        <tr>
          <th class="title"> .....  </th>
        </tr>
        <tr>
          <th class="title">Timestep</th>
          <td class="space"> </td>
          <td class="data" >{{data_timestep}}</td>
        </tr>
        <tr>
          <th class="title">Time</th>
          <td class="space"> </td>
          <td class="data" >{{data_time}}</td>
        </tr>
        <tr>
          <th class="title">Timezone</th>
          <td class="space"> </td>
          <td class="data" >{{data_zone}}</td>
        </tr>
      </table>
    </body>
  </html>)=====";

  header_data.replace("{{deviceName}}", DeviceName);
  header_data.replace("{{data_gas}}", String(bmedata.gasPercentage));
  header_data.replace("{{data_breahtvoc}}", String(bmedata.breathVocEquivalent));
  header_data.replace("{{data_pressure}}", String(bmedata.pressure));
  header_data.replace("{{data_timestep}}", String(bmedata.outputTimestamp));
  // header_data.replace("{{data_time}}",data_time);
  header_data.replace("{{data_zone}}", TIMEZONE);
  header_data.replace("{{data_time}}", acDate);

  header_data.replace("{{data_iaqaccuracy}}", String(bmedata.iaqAccuracy));
  switch (bmedata.iaqAccuracy)
  {
  case 0:
  {
    header_data.replace("{{color_iaqaccuracy}}", "777");
    header_data.replace("{{descr_iaqaccuracy}}", "Calibration phase. Please wait....");
    break;
  }
  case 1:
  {
    header_data.replace("{{color_iaqaccuracy}}", "FFFF00");
    header_data.replace("{{descr_iaqaccuracy}}", "learning");
    break;
  }
  case 2:
  {
    header_data.replace("{{color_iaqaccuracy}}", "ADFF2F");
    header_data.replace("{{descr_iaqaccuracy}}", "good");
    break;
  }
  case 3:
  {
    header_data.replace("{{color_iaqaccuracy}}", "006B3C");
    header_data.replace("{{descr_iaqaccuracy}}", "good. start saving them.");
    break;
  }
  default:
  {
    header_data.replace("{{color_iaqaccuracy}}", "FF0000");
    header_data.replace("{{descr_iaqaccuracy}}", "this should never happen! iaqAccuracy < 0 || iaqAccuracy > 3");
  }
  }

  header_data.replace("{{data_temp}}", String(bmedata.temperature));
  if (bmedata.temperature)
  {
    if (bmedata.temperature < 16) // too cold
    {
      header_data.replace("{{color_temp}}", "add8e6"); // LightBlue
      header_data.replace("{{descr_temp}}", "too cold");
    }
    else if (bmedata.temperature < 18) // cold
    {
      header_data.replace("{{color_temp}}", "0000ff"); // Blue
      header_data.replace("{{descr_temp}}", "cold");
    }
    else if (bmedata.temperature < 20) // cool
    {
      header_data.replace("{{color_temp}}", "2e8b57"); // SeaGreen
      header_data.replace("{{descr_temp}}", "cool");
    }
    else if (bmedata.temperature < 22) // normal
    {
      header_data.replace("{{color_temp}}", "00ff00"); // Green
      header_data.replace("{{descr_temp}}", "normal");
    }
    else if (bmedata.temperature < 24) // cosy
    {
      header_data.replace("{{color_temp}}", "adff2f"); // GreenYellow
      header_data.replace("{{descr_temp}}", "cosy");
    }
    else if (bmedata.temperature < 26) // warm
    {
      header_data.replace("{{color_temp}}", "ffff00"); // Yellow
      header_data.replace("{{descr_temp}}", "warm");
    }
    else if (bmedata.temperature < 28) // hot
    {
      header_data.replace("{{color_temp}}", "ffa500"); // orange
      header_data.replace("{{descr_temp}}", "hot");
    }
    else if (bmedata.temperature > 28) // scalding hot
    {
      header_data.replace("{{color_temp}}", "ff0000"); // Red
      header_data.replace("{{descr_temp}}", "scalding hot");
    }
    else
    {
      header_data.replace("{{color_temp}}", "ff00ff"); // Magenta
      header_data.replace("{{descr_temp}}", "way too hot");
    }
  }

  header_data.replace("{{data_relativehumidity}}", String(bmedata.humidity));
  if (bmedata.humidity)
  {
    if (bmedata.humidity < 20) // Far too dry
    {
      header_data.replace("{{color_relativehumidity}}", "ff0000"); // Red
      header_data.replace("{{descr_relativehumidity}}", "Far too dry");
    }
    else if (bmedata.humidity < 30) // Too dry
    {
      header_data.replace("{{color_relativehumidity}}", "ffff00"); // Yellow
      header_data.replace("{{descr_relativehumidity}}", "too dry");
    }
    else if (bmedata.humidity < 40) // dry
    {
      header_data.replace("{{color_relativehumidity}}", "adff2f"); // GreenYellow
      header_data.replace("{{descr_relativehumidity}}", "dry");
    }
    else if (bmedata.humidity < 50) // normal
    {
      header_data.replace("{{color_relativehumidity}}", "00ff00"); // Green
      header_data.replace("{{descr_relativehumidity}}", "normal");
    }
    else if (bmedata.humidity < 60) // Slightly moist
    {
      header_data.replace("{{color_relativehumidity}}", "adff2f"); // YellowGreen
      header_data.replace("{{descr_relativehumidity}}", "Slightly moist");
    }
    else if (bmedata.humidity < 65) // moist
    {
      header_data.replace("{{color_relativehumidity}}", "ffa500"); // Orange
      header_data.replace("{{descr_relativehumidity}}", "moist");
    }
    else if (bmedata.humidity < 70) // very moist
    {
      header_data.replace("{{color_relativehumidity}}", "ff0000"); // Red
      header_data.replace("{{descr_relativehumidity}}", "very moist");
    }
    else // wet
    {
      header_data.replace("{{descr_relativehumidity}}", "wet");
      header_data.replace("{{color_relativehumidity}}", "ff00ff"); // Magenta
    }
  }

  header_data.replace("{{data_iaq}}", String(bmedata.iaq));
  if (bmedata.iaq > 0)
  {
    if (bmedata.iaq <= 50) // exellent
    {
      header_data.replace("{{color_iaq}}", "2e8b57"); // SeaGreen
      header_data.replace("{{descr_iaq}}", "exellent");
    }
    else if (bmedata.iaq <= 100) // good
    {
      header_data.replace("{{color_iaq}}", "00ff00"); // Green
      header_data.replace("{{descr_iaq}}", "good");
    }
    else if (bmedata.iaq <= 150) // lightly polluted. Ventilation suggested.
    {
      header_data.replace("{{color_iaq}}", "9acd32"); // YellowGreen
      header_data.replace("{{descr_iaq}}", "lightly polluted. Ventilation suggested.");
    }
    else if (bmedata.iaq <= 200) // moderately polluted. please ventilate.
    {
      header_data.replace("{{color_iaq}}", "ffff00"); // Yellow
      header_data.replace("{{descr_iaq}}", "moderately polluted. please ventilate.");
    }
    else if (bmedata.iaq < 250) // heavily polluted. please ventilate.
    {
      header_data.replace("{{color_iaq}}", "ffa500"); // orange
      header_data.replace("{{descr_iaq}}", "heavily polluted. please ventilate.");
    }
    else if (bmedata.iaq < 300) // severly polluted. please ventilate urgently.
    {
      header_data.replace("{{color_iaq}}", "ff0000"); // Red
      header_data.replace("{{descr_iaq}}", "severly polluted. please ventilate urgently.");
    }
    else // extremly polluted. please ventilate urgently.
    {
      header_data.replace("{{descr_iaq}", "extremly polluted. please ventilate urgently.");
      header_data.replace("{{color_iaq}}", "ff00ff"); // Magenta
    }
  }

  header_data.replace("{{data_MHZ19B_co2}}", String(co2data.getRegular()));
  if (co2data.getRegular() > 0)
  {
    if (co2data.getRegular() < 600) // outdoor air
    {
      header_data.replace("{]descr_MHZ19B_co2}}", "outdoor air");
      header_data.replace("{{color_MHZ19B_co2}}", "0000ff"); // Blue
    }
    else if (co2data.getRegular() < 800) // fresh indoor air
    {
      header_data.replace("{{descr_MHZ19B_co2}}", "fresh indoor air");
      header_data.replace("{{color_MHZ19B_co2}}", "00ff00"); // Green
    }
    else if (co2data.getRegular() < 1000) // Indoor air
    {
      header_data.replace("{{descr_MHZ19B_co2}}", "Indoor air");
      header_data.replace("{{color_MHZ19B_co2}}", "adff2f"); // GreenYellow
    }
    else if (co2data.getRegular() < 1200) // used indoor air. please ventilate
    {
      header_data.replace("{{descr_MHZ19B_co2}}", "used indoor air. please ventilate");
      header_data.replace("{{color_MHZ19B_co2}}", "ffff00"); // Yellow
    }
    else if (co2data.getRegular() < 1400) // stale indoor air. please ventilate
    {
      header_data.replace("{{descr_MHZ19B_co2}}", "stale indoor air. please ventilate");
      header_data.replace("{{color_MHZ19B_co2}}", "ffa500"); // Orange
    }
    else if (co2data.getRegular() < 1600) // strongly stale indoor air. please ventilate urgently. thinking performance impaired
    {
      header_data.replace("{{descr_MHZ19B_co2}}", "strongly stale indoor air. please ventilate urgently. thinking performance impaired");
      header_data.replace("{{color_MHZ19B_co2}}", "ff0000"); // Red
    }
    else // Tiredness, headache. please ventilate urgently.
    {
      header_data.replace("{{descr_MHZ19B_co2}}", "Warning. Tiredness, headache. please ventilate urgently.");
      header_data.replace("{{color_MHZ19B_co2}}", "ff00ff"); // Magenta
    }
  }

  request->send(200, "text/html; charset=utf-8", header_data);
}

void WebServerHandler::handle_NotFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain; charset=utf-8", "Not found");
}

void WebServerHandler::setInputDataforBody(DataCO2 co2Sensordata, Bsec enviromentdata, String sdate)
{
  this->bmedata = enviromentdata;
  this->co2data = co2Sensordata;
  acDate = sdate;
}

void WebServerHandler::start()
{
  server.begin();
}
