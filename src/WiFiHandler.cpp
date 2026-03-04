#include <WiFi.h>
#include "WiFiHandler.h"
#include "mdns.h"
// please rename credentials_example.h to credentials.h and set your WIFI Credentials there
#include "Credentials.h"
#include "Configuration.h"
#include "ConfigHandler.h"
extern ConfigHandler &configHandler;
unsigned long WiFiHandler::_lastRunSeconds = 0;
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
	const char *apSSID = AP_SSID;
    const char *apPassword = AP_PASSWORD;

	WiFi.softAP(apSSID, apPassword);
	Serial.println("[AP MODE] no known wifi credentials found, starting AP mode");
	Serial.print("[AP MODE] IP address: ");
	Serial.println(WiFi.softAPIP());
}

void WiFiHandler::initWifi()
{
	int wifiWaitCount = 0;
	// set device for mDNS and hostname
	String hostname = configHandler.getConfigDevice("deviceName");
	hostname.replace(" ", "-");
	hostname.toLowerCase();
	WiFi.setHostname(hostname.c_str());
	WiFi.setAutoReconnect(true);
	WiFi.persistent(true);
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
		String mdnsName = configHandler.getConfigDevice("deviceName");
		mdnsName.replace(" ", "-");
		if (mdns_init() == ESP_OK)
		{
			mdns_hostname_set(mdnsName.c_str());
			Serial.println("[WIFI] mDNS gestartet: http://" + mdnsName + ".local");
		}
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
	WiFi.disconnect(true);
	delay(100);
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
	if (currentSeconds - _lastRunSeconds >= (unsigned long)configHandler.getConfigInterval("intervalWiFi"))
	{
		_lastRunSeconds = currentSeconds;
		return WiFiHandler::StatusCheck();
	}
	return false;
}
