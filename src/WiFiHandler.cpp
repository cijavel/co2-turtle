//
// Created by Armin on 18.05.2023.
//

#include <WiFi.h>
#include "WiFiHandler.h"
#include "Credentials.h"
#include "Configuration.h"

void WiFiHandler::initWifi() {
    int wifiWaitCount = 0;
    WiFiClass::setHostname(DeviceName); //maybe cstr
    Serial.print("\nWIFI: Connecting to ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PW);

    while (WiFiClass::status() != WL_CONNECTED && wifiWaitCount < 20)
    {
        delay(250);
        wifiWaitCount++;
    }

    if (WiFiClass::status() == WL_CONNECTED)
    {
        Serial.println("WIFI: connected.");
        Serial.print("WIFI: IP address: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("WIFI: not connected");
    }
}
bool WiFiHandler::StatusCheck()
{
    if (WiFiClass::status() != WL_CONNECTED)
    {
        ReStart();
    }
    else
    {
        Serial.println("WIFI: Still connected");
    }
    return WiFiClass::status() == WL_CONNECTED;
}

void WiFiHandler::ReStart()
{
    // Connect to WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PW);
    int wifiWaitCount = 0;
    while (WiFiClass::status() != WL_CONNECTED && wifiWaitCount < 20)
    {
        delay(250);
        wifiWaitCount++;
    }
    if (WiFiClass::status() == WL_CONNECTED)
    {
        Serial.println("");
        Serial.println("WiFi connected");

        // Print the IP address
        Serial.println(WiFi.localIP());
    }
}

bool WiFiHandler::checkWifi(unsigned long currentSeconds) {
    if (currentSeconds % interval_WiFiCheck_in_Seconds == 0) {
        return WiFiHandler::StatusCheck();
    }
    return false;
}

