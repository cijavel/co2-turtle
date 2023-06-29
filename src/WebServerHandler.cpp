#include "WebServerHandler.h"
#include "Configuration.h"

WebServerHandler::WebServerHandler()
{
  server.on("/", HTTP_GET, handle_index);
  server.on("/json", HTTP_GET, std::bind(&WebServerHandler::handle_data, this, std::placeholders::_1));
  server.on("/status", HTTP_GET, std::bind(&WebServerHandler::handle_status, this, std::placeholders::_1));
  server.onNotFound(handle_NotFound);
}

void WebServerHandler::handle_index(AsyncWebServerRequest *request)
{
  String content = R"=====(
  <!DOCTYPE HTML>
    <html>
      <head>
        <title>Index</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
      </head>
      <body>
        <div style="font-family: Arial, Helvetica, sans-serif;">
          <div style="padding: 5 px;">
            <a href="/status">status</a>
          </div>
          <div style="padding: 5 px;">
            <a href="/json">json file</a>
          </div>
        </div>
      </body>
    </html>
  )=====";
  request->send(200, "text/html; charset=utf-8", content);
}

void WebServerHandler::handle_data(AsyncWebServerRequest *request)
{
  String header_data = "";
  //            "{\n\"" +
  //            name_timestamp        + "\":\"" + data_timestamp              + "\",\n" + "\"" +
  //            name_datetime         + "\":\"" + data_date + " " + data_time + "\",\n" + "\"" +
  //            name_zone             + "\":\"" + data_zone                   + "\",\n" + "\"" +
  //            name_rawtemperatur    + "\":\"" + data_rawtemperatur          + "\",\n" + "\"" +
  //            name_temp             + "\":\"" + data_temp                   + "\",\n" + "\"" +
  //            name_pressure         + "\":\"" + data_pressure               + "\",\n" + "\"" +
  //            name_rawhumidity      + "\":\"" + data_rawhumidity            + "\",\n" + "\"" +
  //            name_relativehumidity + "\":\"" + data_relativehumidity       + "\",\n" + "\"" +
  //            name_gas              + "\":\"" + data_gas                    + "\",\n" + "\"" +
  //            name_iaq              + "\":\"" + data_iaq                    + "\",\n" + "\"" +
  //            name_iaqaccuracy      + "\":\"" + data_iaqaccuracy            + "\",\n" + "\"" +
  //            name_iaqstatic        + "\":\"" + data_iaqstatic              + "\",\n" +  "\"" +
  //            name_co2equil         + "\":\"" + data_co2equil               + "\",\n" + "\"" +
  //            name_breahtvoc        + "\":\"" + data_breahtvoc              + "\",\n" + "\"" +
  //            name_MHZ19B_co2       + "\":\"" + data_MHZ19B_co2             + "\"\n}";
  request->send(200, "application/json; charset=utf-8", header_data);
}

void WebServerHandler::handle_status(AsyncWebServerRequest *request)
{
  String header_data = R"=====(<!DOCTYPE HTML>
  <html>
    <head>
      <style>
        .status {
            border-radius: 50%;
            padding:0px 10px;
        }
      </style>
      <title>sensor status</title>
      <meta name="viewport" content="width=device-width, initial-scale=1">
    </head>

    <body style="font-family: Arial, Helvetica, sans-serif;">
      <h1 style="font-size: 120%;">{deviceName}</h1>
      <table class="content" style="text-align: left;">
        <tr>
          <th class="title">Sensor Accuracy</th>
          <td class="space"> </td>
          <td class="data" style="padding:0px 5px">{data_iaqaccuracy} level</td>
          <td class="space"> </td>
          <td class="status" style="background-color: #{color_iaqaccuracy};"> </td>
          <td class="space"> </td>
          <td class="description" style="padding:0px 5px">{descr_iaqaccuracy}</td>
        </tr>
        <tr>
          <th class="title">Temperature</th>
          <td class="space"> </td>
          <td class="data" style="padding:0px 5px">{data_temp} °C</td>
          <td class="space"> </td>
          <td class="status" style="background-color: #{color_temp}; "> </td>
          <td class="space"> </td>
          <td class="data" style="padding:0px 5px">{descr_temp}</td>
        </tr>
        <tr>
          <th class="title">Humidity</th>
          <td class="space"> </td>
          <td class="data" style="padding:0px 5px">{data_relativehumidity} %</td>
          <td class="space"> </td>
          <td class="status" style="background-color: #{color_relativehumidity};"> </td>
          <td class="space"> </td>
          <td class="data" style="padding:0px 5px">{descr_relativehumidity}</td>
        </tr>
        <tr>
          <th class="title">Air Quality </th>
          <td class="space"> </td>
          <td class="data" style="padding:0px 5px">{data_iaq} IAQ</td>
          <td class="space"> </td>
          <td class="status" style="background-color: #{color_iaq};"> </td>
          <td class="space"> </td>
          <td class="data" style="padding:0px 5px">{descr_iaq}</td>
        </tr>
        <tr>
          <th class="title">Co2 Level</th>
          <td class="space"> </td>
          <td class="data" style="padding:0px 5px">{data_MHZ19B_co2} ppm</td>
          <td class="space"> </td>
          <td class="status" style="background-color: #{color_MHZ19B_co2};"> </td>
          <td class="space"> </td>
          <td class="data" style="padding:0px 5px">{descr_MHZ19B_co2}</td>
        </tr>
        <tr>
          <th class="title"> .....  </th>
        </tr>
         <tr>
          <th class="title">Air pressure</th>
          <td class="space"> </td>
          <td class="data" style="padding:0px 5px">{data_pressure} hPa</td>
        </tr>
        <tr>
          <th class="title">Gas resistance</th>
          <td class="space"> </td>
          <td class="data" style="padding:0px 5px">{data_gas} Ohm</td>
        </tr>
        <tr>
          <th class="title">breath VOC Equivalent</th>
          <td class="space"> </td>
          <td class="data" style="padding:0px 5px">{data_breahtvoc} ppm</td>
        </tr>
        <tr>
          <th class="title"> .....  </th>
        </tr>
        <tr>
          <th class="title">Date</th>
          <td class="space"> </td>
          <td class="data" style="padding:0px 5px">{data_date}</td>
        </tr>
        <tr>
          <th class="title">Time</th>
          <td class="space"> </td>
          <td class="data" style="padding:0px 5px">{data_time}</td>
        </tr>
        <tr>
          <th class="title">Timezone</th>
          <td class="space"> </td>
          <td class="data" style="padding:0px 5px">{data_zone}</td>
        </tr>
      </table>
    </body>
  </html>)=====";

  header_data.replace("{deviceName}", DeviceName);
  header_data.replace("{data_gas}", String(bmedata.gasPercentage));
  header_data.replace("{data_breahtvoc}", String(bmedata.breathVocEquivalent));
  header_data.replace("{data_pressure}", String(bmedata.pressure));
  // header_data.replace("{data_date}",data_date);
  // header_data.replace("{data_time}",data_time);
  // header_data.replace("{data_zone}",data_zone);

  header_data.replace("{data_iaqaccuracy}", String(bmedata.iaqAccuracy));
  switch (bmedata.iaqAccuracy)
  {
  case 0:
  {
    header_data.replace("{color_iaqaccuracy}", "777");
    header_data.replace("{descr_iaqaccuracy}", "Calibration phase. Please wait....");
    break;
  }
  case 1:
  {
    header_data.replace("{color_iaqaccuracy}", "FFFF00");
    header_data.replace("{descr_iaqaccuracy}", "learning");
    break;
  }
  case 2:
  {
    header_data.replace("{color_iaqaccuracy}", "ADFF2F");
    header_data.replace("{descr_iaqaccuracy}", "good");
    break;
  }
  case 3:
  {
    header_data.replace("{color_iaqaccuracy}", "006B3C");
    header_data.replace("{descr_iaqaccuracy}", "good. start saving them.");
    break;
  }
  default:
  {
    header_data.replace("{color_iaqaccuracy}", "FF0000");
    header_data.replace("{descr_iaqaccuracy}", "this should never happen! iaqAccuracy < 0 || iaqAccuracy > 3");
  }
  }

  header_data.replace("{data_temp}", String(bmedata.temperature));
  if (bmedata.temperature)
  {
    if (bmedata.temperature < 16) // too cold
    {
      header_data.replace("{color_temp}", "add8e6"); // LightBlue
      header_data.replace("{descr_temp}", "too cold");
    }
    else if (bmedata.temperature < 18) // cold
    {
      header_data.replace("{color_temp}", "0000ff"); // Blue
      header_data.replace("{descr_temp}", "cold");
    }
    else if (bmedata.temperature < 20) // cool
    {
      header_data.replace("{color_temp}", "2e8b57"); // SeaGreen
      header_data.replace("{descr_temp}", "cool");
    }
    else if (bmedata.temperature < 22) // normal
    {
      header_data.replace("{color_temp}", "00ff00"); // Green
      header_data.replace("{descr_temp}", "normal");
    }
    else if (bmedata.temperature < 24) // cosy
    {
      header_data.replace("{color_temp}", "adff2f"); // GreenYellow
      header_data.replace("{descr_temp}", "cosy");
    }
    else if (bmedata.temperature < 26) // warm
    {
      header_data.replace("{color_temp}", "ffff00"); // Yellow
      header_data.replace("{descr_temp}", "warm");
    }
    else if (bmedata.temperature < 28) // hot
    {
      header_data.replace("{color_temp}", "ffa500"); // orange
      header_data.replace("{descr_temp}", "hot");
    }
    else if (bmedata.temperature > 28) // scalding hot
    {
      header_data.replace("{color_temp}", "ff0000"); // Red
      header_data.replace("{descr_temp}", "scalding hot");
    }
    else
    {
      header_data.replace("{color_temp}", "ff00ff"); // Magenta
      header_data.replace("{descr_temp}", "way too hot");
    }
  }

  header_data.replace("{data_relativehumidity}", String(bmedata.humidity));
  if (bmedata.humidity)
  {
    if (bmedata.humidity < 20) // Far too dry
    {
      header_data.replace("{color_relativehumidity}", "ff0000"); // Red
      header_data.replace("{descr_relativehumidity}", "Far too dry");
    }
    else if (bmedata.humidity < 30) // Too dry
    {
      header_data.replace("{color_relativehumidity}", "ffff00"); // Yellow
      header_data.replace("{descr_relativehumidity}", "too dry");
    }
    else if (bmedata.humidity < 40) // dry
    {
      header_data.replace("{color_relativehumidity}", "adff2f"); // GreenYellow
      header_data.replace("{descr_relativehumidity}", "dry");
    }
    else if (bmedata.humidity < 50) // normal
    {
      header_data.replace("{color_relativehumidity}", "00ff00"); // Green
      header_data.replace("{descr_relativehumidity}", "normal");
    }
    else if (bmedata.humidity < 60) // Slightly moist
    {
      header_data.replace("{color_relativehumidity}", "adff2f"); // YellowGreen
      header_data.replace("{descr_relativehumidity}", "Slightly moist");
    }
    else if (bmedata.humidity < 65) // moist
    {
      header_data.replace("{color_relativehumidity}", "ffa500"); // Orange
      header_data.replace("{descr_relativehumidity}", "moist");
    }
    else if (bmedata.humidity < 70) // very moist
    {
      header_data.replace("{color_relativehumidity}", "ff0000"); // Red
      header_data.replace("{descr_relativehumidity}", "very moist");
    }
    else // wet
    {
      header_data.replace("{descr_relativehumidity}", "wet");
      header_data.replace("{color_relativehumidity}", "ff00ff"); // Magenta
    }
  }

  header_data.replace("{data_iaq}", String(bmedata.iaq));
  if (bmedata.iaq > 0)
  {
    if (bmedata.iaq <= 50) // exellent
    {
      header_data.replace("{color_iaq}", "2e8b57"); // SeaGreen
      header_data.replace("{descr_iaq}", "exellent");
    }
    else if (bmedata.iaq <= 100) // good
    {
      header_data.replace("{color_iaq}", "00ff00"); // Green
      header_data.replace("{descr_iaq}", "good");
    }
    else if (bmedata.iaq <= 150) // lightly polluted. Ventilation suggested.
    {
      header_data.replace("{color_iaq}", "9acd32"); // YellowGreen
      header_data.replace("{descr_iaq}", "lightly polluted. Ventilation suggested.");
    }
    else if (bmedata.iaq <= 200) // moderately polluted. please ventilate.
    {
      header_data.replace("{color_iaq}", "ffff00"); // Yellow
      header_data.replace("{descr_iaq}", "moderately polluted. please ventilate.");
    }
    else if (bmedata.iaq < 250) // heavily polluted. please ventilate.
    {
      header_data.replace("{color_iaq}", "ffa500"); // orange
      header_data.replace("{descr_iaq}", "heavily polluted. please ventilate.");
    }
    else if (bmedata.iaq < 300) // severly polluted. please ventilate urgently.
    {
      header_data.replace("{color_iaq}", "ff0000"); // Red
      header_data.replace("{descr_iaq}", "severly polluted. please ventilate urgently.");
    }
    else // extremly polluted. please ventilate urgently.
    {
      header_data.replace("{descr_iaq}", "extremly polluted. please ventilate urgently.");
      header_data.replace("{color_iaq}", "ff00ff"); // Magenta
    }
  }

  header_data.replace("{data_MHZ19B_co2}", String(co2data.getRegular()));
  if (co2data.getRegular() > 0)
  {
    if (co2data.getRegular() < 600) // outdoor air
    {
      header_data.replace("{descr_MHZ19B_co2}", "outdoor air");
      header_data.replace("{color_MHZ19B_co2}", "0000ff"); // Blue
    }
    else if (co2data.getRegular() < 800) // fresh indoor air
    {
      header_data.replace("{descr_MHZ19B_co2}", "fresh indoor air");
      header_data.replace("{color_MHZ19B_co2}", "00ff00"); // Green
    }
    else if (co2data.getRegular() < 1000) // Indoor air
    {
      header_data.replace("{descr_MHZ19B_co2}", "Indoor air");
      header_data.replace("{color_MHZ19B_co2}", "adff2f"); // GreenYellow
    }
    else if (co2data.getRegular() < 1200) // used indoor air. please ventilate
    {
      header_data.replace("{descr_MHZ19B_co2}", "used indoor air. please ventilate");
      header_data.replace("{color_MHZ19B_co2}", "ffff00"); // Yellow
    }
    else if (co2data.getRegular() < 1400) // stale indoor air. please ventilate
    {
      header_data.replace("{descr_MHZ19B_co2}", "stale indoor air. please ventilate");
      header_data.replace("{color_MHZ19B_co2}", "ffa500"); // Orange
    }
    else if (co2data.getRegular() < 1600) // strongly stale indoor air. please ventilate urgently. thinking performance impaired
    {
      header_data.replace("{descr_MHZ19B_co2}", "strongly stale indoor air. please ventilate urgently. thinking performance impaired");
      header_data.replace("{color_MHZ19B_co2}", "ff0000"); // Red
    }
    else // Tiredness, headache. please ventilate urgently.
    {
      header_data.replace("{descr_MHZ19B_co2}", "Warning. Tiredness, headache. please ventilate urgently.");
      header_data.replace("{color_MHZ19B_co2}", "ff00ff"); // Magenta
    }
  }

  request->send(200, "text/html; charset=utf-8", header_data);
}

void WebServerHandler::handle_NotFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain; charset=utf-8", "Not found");
}

void WebServerHandler::setCo2AndData(CO2Data co2Sensordata, Bsec enviromentdata)
{
  this->bmedata = enviromentdata;
  this->co2data = co2Sensordata;
}

void WebServerHandler::start()
{
  server.begin();
}
