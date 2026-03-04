#include <WiFi.h>
#include "WiFiHandler.h"
#include "mdns.h"
// please rename credentials_example.h to credentials.h and set your WIFI Credentials there
#include "Credentials.h"
#include "Configuration.h"
#include "ConfigHandler.h"
extern ConfigHandler &configHandler;
unsigned long WiFiHandler::_lastRunSeconds = 0;
static String _ssid;
static String _password;

void WiFiHandler::loadWiFiCredentials()
{
    _ssid     = configHandler.getConfigDevice("wlanSSID");
    _password = configHandler.getConfigDevice("wlanPASSWORD");
	Serial.println("[WIFI] get credentails");
}

static bool connectToWifi()
{
    if (_ssid.isEmpty())
    {
        Serial.println("[WIFI] No SSID configured, skipping connection attempt");
        return false;
    }

    WiFi.begin(_ssid.c_str(), _password.c_str());

    int attempts = 0;
    while (WiFiClass::status() != WL_CONNECTED && attempts < 20)
    {
        delay(250);
        attempts++;
    }

    bool connected = (WiFiClass::status() == WL_CONNECTED);
    if (connected)
        Serial.println("[WIFI] Connected. IP: " + WiFi.localIP().toString());
    else
        Serial.println("[WIFI] Connection failed after " + String(attempts) + " attempts");

    return connected;
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
	WiFi.begin(_ssid.c_str(), _password.c_str());

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
	WiFi.begin(_ssid.c_str(), _password.c_str());
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
    bool connected = (WiFiClass::status() == WL_CONNECTED);
    if (!connected)
    {
        Serial.println("[WIFI] Connection lost, reconnecting...");
        ReStart();
        connected = (WiFiClass::status() == WL_CONNECTED);
    }
    return connected;
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
