#include "WebServerHandler.h"
#include "Configuration.h"

WebServerHandler::WebServerHandler() {
    server.on("/", HTTP_GET, handle_index);
    server.on("/json", HTTP_GET, std::bind(&WebServerHandler::handle_data, this, std::placeholders::_1));
    server.on("/status", HTTP_GET, std::bind(&WebServerHandler::handle_status, this, std::placeholders::_1));
    server.onNotFound(handle_NotFound);
}

void WebServerHandler::handle_index(AsyncWebServerRequest *request) {
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

void WebServerHandler::handle_data(AsyncWebServerRequest *request) {
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

void WebServerHandler::handle_status(AsyncWebServerRequest *request) {
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
          <th class="title">VOC</th>
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
    header_data.replace("{data_iaqaccuracy}", String(data->iaqAccuracy));
    switch (data->iaqAccuracy) {
        case 0:{
            header_data.replace("{color_iaqaccuracy}", "777");
            header_data.replace("{descr_iaqaccuracy}", "Calibration phase. Please wait....");
            break;
        }
        case 1: {
            header_data.replace("{color_iaqaccuracy}","FFFF00");
            header_data.replace("{descr_iaqaccuracy}", "learning");
            break;
        }
        case 2: {
            header_data.replace("{color_iaqaccuracy}","ADFF2F");
            header_data.replace("{descr_iaqaccuracy}", "good");
            break;
        }
        case 3: {
            header_data.replace("{color_iaqaccuracy}", "006B3C");
            header_data.replace("{descr_iaqaccuracy}", "good. start saving them.");
            break;
        }
        default: {
            header_data.replace("{color_iaqaccuracy}", "FF0000");
            header_data.replace("{descr_iaqaccuracy}", "this should never happen! iaqAccuracy < 0 || iaqAccuracy > 3");
        }
    }
//    header_data.replace("{data_temp}",data_temp);
//    header_data.replace("{color_temp}",color_temp);
//    header_data.replace("{descr_temp}",descr_temp);
//    header_data.replace("{data_relativehumidity}",data_relativehumidity);
//    header_data.replace("{color_relativehumidity}",color_relativehumidity);
//    header_data.replace("{descr_relativehumidity}",descr_relativehumidity);
//    header_data.replace("{data_iaq}",data_iaq);
//    header_data.replace("{color_iaq}",color_iaq);
//    header_data.replace("{descr_iaq}",descr_iaq);
//    header_data.replace("{data_MHZ19B_co2}",data_MHZ19B_co2);
//    header_data.replace("{color_MHZ19B_co2}",color_MHZ19B_co2);
//    header_data.replace("{descr_MHZ19B_co2}",descr_MHZ19B_co2);
//    header_data.replace("{data_pressure}",data_pressure);
//    header_data.replace("{data_gas}",data_gas);
//    header_data.replace("{data_breahtvoc}",data_breahtvoc);
//    header_data.replace("{data_date}",data_date);
//    header_data.replace("{data_time}",data_time);
//    header_data.replace("{data_zone}",data_zone);
//    header_data.replace("{deviceName}",deviceName);

    request->send(200, "text/html; charset=utf-8", header_data);
}

void WebServerHandler::handle_NotFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain; charset=utf-8", "Not found");
}

void WebServerHandler::setCo2AndData(CO2Data *co2Sensordata, Bsec *enviromentdata) {
    this->data = enviromentdata;
    this->co2data=co2Sensordata;
}

void WebServerHandler::start() {
    server.begin();
}
