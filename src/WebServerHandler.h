#ifndef CO2_TURTLE_WEBSERVERHANDLER_H
#define CO2_TURTLE_WEBSERVERHANDLER_H

#include "ESPAsyncWebServer.h"
#include "DataCO2.h"
#include "bsec.h"

class WebServerHandler {
public:
    static WebServerHandler &getInstance() {
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
    WebServerHandler(const WebServerHandler&) = delete;
    WebServerHandler& operator=(const WebServerHandler&) = delete;

    // Request handlers
    static void handle_index(AsyncWebServerRequest* request);
    void handle_data(AsyncWebServerRequest* request);
    void handle_status(AsyncWebServerRequest* request);
    void handle_ap(AsyncWebServerRequest* request);
    void handle_credentials_submit(AsyncWebServerRequest* request);
    static void handle_NotFound(AsyncWebServerRequest* request);
};
#endif //CO2_TURTLE_WEBSERVERHANDLER_H
