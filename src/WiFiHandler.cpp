#include <WiFi.h>
#include "WiFiHandler.h"
// please rename credentials_example.h to credentials.h and set your WIFI Credentials there
#include "Credentials.h"
#include "Configuration.h"
#include "ConfigHandler.h"
extern ConfigHandler configHandler;
String password;
String ssid;

void WiFiHandler::loadWiFiCredentials()
{
	ssid = configHandler.getConfigDevice("wlanSSID");
	password = configHandler.getConfigDevice("wlanPASSWORD");
	Serial.println("[WIFI] get credentails");
}

void WiFiHandler::setupAPMode()
{
	const char *apSSID = "sensorturle 192.168.4.1";
	const char *apPassword = "sensorturtle";

	WiFi.softAP(apSSID, apPassword);
	Serial.println("[AP MODE] no known wifi credentials found, starting AP mode");
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
		Serial.print("[WIFI] connected. IP address: ");
		Serial.println(WiFi.localIP());
	}
	else
	{
		Serial.println("[WIFI] Starting AP mode. Please connect to the esp32 wlan");
		setupAPMode();
	}
}

void WiFiHandler::ReStart()
{
#ifdef DEBUG
	Serial.println();
	Serial.print("[WIFI] Connecting to: ");
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
		Serial.print("[WIFI] still connected. IP: ");
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
		Serial.println("[WIFI] restarting WiFi connection");
	}
	return status == WL_CONNECTED;
}

bool WiFiHandler::checkWifiStatus(unsigned long currentSeconds)
{
	if (currentSeconds % configHandler.getConfigInterval("intervalWiFi") == 0)
	{
		return WiFiHandler::StatusCheck();
	}
	return false;
}
