#include <WiFi.h>
#include "WiFiHandler.h"
// please rename credentials_example.h to credentials.h and set your WIFI Credentials there
#include <Preferences.h>
#include "Credentials.h"
#include "Configuration.h"
#include "WebServerHandler.h"
extern Preferences preferences;
String password;
String ssid;

void WiFiHandler::loadWiFiCredentials()
{
    preferences.begin("wifi", true);

    ssid = preferences.getString("ssid", "");
    password = preferences.getString("password", "");
    preferences.end();

    if (ssid == "" || password == "")
    {
        ssid = WIFI_SSID;
        password = WIFI_PW;
        Serial.println("using default credentials. No stored WiFi credentials found");
    }
    else
    {
        Serial.println("Stored WiFi credentials loaded.");
    }

    Serial.print("SSID: ");
    Serial.println(ssid);
}

void WiFiHandler::setupAPMode()
{
    const char *apSSID = "sensorturle 192.168.4.1";
    const char *apPassword = "sensorturtle";

    WiFi.softAP(apSSID, apPassword);
    Serial.println("[AP MODE] Access Point Started");
    Serial.print("[AP MODE] IP address: ");
    Serial.println(WiFi.softAPIP());
}

void WiFiHandler::initWifi()
{
    int wifiWaitCount = 0;
    WiFiClass::setHostname(DeviceName);
#ifdef DEBUG
    Serial.print("\n[WIFI] Connecting to ");
    Serial.println(WIFI_SSID);
#endif
    loadWiFiCredentials();
    WiFi.begin(ssid.c_str(), password.c_str());

    while (WiFiClass::status() != WL_CONNECTED && wifiWaitCount < 20)
    {
        delay(250);
        wifiWaitCount++;
    }
    if (WiFiClass::status() == WL_CONNECTED)
    {
        Serial.println("[WIFI] connected.");
        Serial.print("[WIFI] IP address: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("[WIFI] Could not connect. Starting AP mode. Please connect to the wlan, which was created by the esp32");
        setupAPMode();
    }
}

void WiFiHandler::ReStart()
{
#ifdef DEBUGk
    Serial.println();
    Serial.print("[WIFI] Connecting to ");
    Serial.println(WIFI_SSID);
#endif
    loadWiFiCredentials();
    WiFi.begin(ssid.c_str(), password.c_str());
    int wifiWaitCount = 0;
    while (WiFiClass::status() != WL_CONNECTED && wifiWaitCount < 20)
    {
        delay(250);
        wifiWaitCount++;
    }
#ifdef DEBUG
    if (WiFiClass::status() == WL_CONNECTED)
    {
        Serial.println();
        Serial.println("[WIFI] WiFi connected");
        Serial.println(WiFi.localIP());
    }
#endif
}

bool WiFiHandler::StatusCheck()
{
    wl_status_t status = WiFiClass::status();
    if (status != WL_CONNECTED)
    {
        ReStart();
    }
#ifdef DEBUG
    else
    {
        Serial.println("[WIFI] Still connected");
        Serial.println();
    }
#endif
    return status == WL_CONNECTED;
}

bool WiFiHandler::checkWifi(unsigned long currentSeconds)
{
    if (currentSeconds % interval_WiFiCheck_in_Seconds == 0)
    {
        return WiFiHandler::StatusCheck();
    }
    return false;
}
