#ifndef CO2_TURTLE_WIFIHANDLER_H
#define CO2_TURTLE_WIFIHANDLER_H

class WiFiHandler
{
public:
	static void initWifi();
	static bool checkWifiStatus(unsigned long currentSeconds);
	static void setupAPMode();

private:
	static bool StatusCheck();
	static void ReStart();
	static void loadWiFiCredentials();
	static unsigned long _lastRunSeconds;
};

#endif // CO2_TURTLE_WIFIHANDLER_H
