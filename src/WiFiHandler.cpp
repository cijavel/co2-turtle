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
	Serial.printf("[WIFI] Credentials loaded: SSID='%s' (%s)\n", _ssid.c_str(), _ssid.isEmpty() ? "empty" : "set");
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
	Serial.printf("[AP MODE] Starting access point '%s', IP: %s\n", AP_SSID, WiFi.softAPIP().toString().c_str());
}

static void setupMDNS()
{
    String name = configHandler.getConfigDevice("deviceName");
    name.replace(" ", "-");
    name.toLowerCase();

    if (mdns_init() == ESP_OK)
    {
        mdns_hostname_set(name.c_str());
        Serial.println("[WIFI] mDNS started: http://" + name + ".local");
    }
}

void WiFiHandler::initWifi()
{
    String hostname = configHandler.getConfigDevice("deviceName");
    if (hostname == String(DEVICE_NAME))
    {
        uint8_t mac[6];
        WiFi.macAddress(mac);
        char suffix[7];
        snprintf(suffix, sizeof(suffix), "%02x%02x%02x", mac[3], mac[4], mac[5]);
        String prefix = String(DEVICE_NAME);
        prefix.replace(" ", "-");
        prefix.toLowerCase();
        hostname = prefix + "-" + suffix;
        configHandler.setConfigDevice("deviceName", hostname);
        Serial.println("[WIFI] Generated unique device name: " + hostname);
    }
    hostname.replace(" ", "-");
    hostname.toLowerCase();
    WiFi.persistent(false);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(hostname.c_str());
    WiFi.setAutoReconnect(true);

    loadWiFiCredentials();

    if (connectToWifi())
    {
        setupMDNS();
    }
    else
    {
        Serial.println("[WIFI] No connection, starting AP mode");
        setupAPMode();
    }
}

void WiFiHandler::ReStart()
{
    loadWiFiCredentials();
    String hostname = configHandler.getConfigDevice("deviceName");
    hostname.replace(" ", "-");
    hostname.toLowerCase();
    WiFi.disconnect(true);
    delay(100);
    WiFi.setHostname(hostname.c_str());
    connectToWifi();
}

bool WiFiHandler::StatusCheck()
{
    bool connected = (WiFiClass::status() == WL_CONNECTED);
    if (!connected)
    {
        Serial.println("[WIFI] Connection lost, reconnecting...");
        ReStart();
        bool reconnected = (WiFiClass::status() == WL_CONNECTED);
    Serial.printf("[WIFI] Reconnect %s%s\n", reconnected ? "OK – IP: " : "FAILED", reconnected ? WiFi.localIP().toString().c_str() : "");
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
