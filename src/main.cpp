#include <Arduino.h>
// _______________
// EPD ePaper eINK
// ---------------
// BUSY 	  -> -1			Violett,
// RST 	  	  -> -1 		RX2	Blau,
// DC 		  -> 27  		TX2	grün,
// CS 		  -> SS(5)		gelb,
// CLK 		  -> SCK(18)	orange,
// DIN /SDI   -> MOSI(23) 	weiß,
// GND 		  -> GND,
// 3.3V 	  -> 3.3V
// #define GxEPD2_DRIVER_CLASS GxEPD2_213_Z98c // GDEY0213Z98 122x250, SSD1680

// _______________
// LED
// ---------------
// DATA -> 4

// _______________
// MH-Z19B
// ---------------
// GND -> GND
// VCC -> 5V
// RX  -> 17
// TX  -> 16

// _______________
// Bosch BME680
// ---------------
// GND -> GND
// VCC -> 3,3-5V
// SCL -> 22
// SDA -> 21
// SD0 -> -
// CS  -> 3,3V

// OOP
#include "BME680Handler.h"
#include "MHZ19Handler.h"
#include "Configuration.h"
#include "Credentials.h"
#include "WiFiHandler.h"
#include <WiFi.h>
#include "bsec.h"
#include <ctime>
#include "EPDHandler.h"
#include "WebServerHandler.h"
#include "LEDHandler.h"
#include "MqttClientHandler.h"
#include "ConfigHandler.h"
#include <LittleFS.h>
ConfigHandler &configHandler = ConfigHandler::getInstance();

// --------------------------------------------------------------------------
// time functions
// --------------------------------------------------------------------------
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;

String localTime(const String &format)
{
	struct tm timeinfo{};

	String time = "";
	char toutp[60];
	String tz = configHandler.getConfigDevice("timezone");
	setenv("TZ", tz.c_str(), 1);
	tzset();

	if (!getLocalTime(&timeinfo))
	{
		time = "TIME: Failed to obtain";
	}
	else
	{
		strftime(toutp, sizeof(toutp), format.c_str(), &timeinfo);
		time = String(toutp);
	}
	return time;
}

void setup()
{
	delay(100);
	Serial.begin(BAUDRATE);
	Serial.println();

	if (!LittleFS.begin())
	{
		Serial.println("Failed to mount LittleFS!");
		return;
	}
	configHandler.setSettingsOnFirstRun();

	WiFiHandler::initWifi();
	configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

	WebServerHandler &webServer = WebServerHandler::getInstance();
	webServer.start();

	if (configHandler.getConfigSwitch("switchLED"))
	{
		LEDHandler &ledhandler = LEDHandler::getInstance();
		ledhandler.setup_led();
	}

	if (configHandler.getConfigSwitch("switchMQTT"))
	{
		MqttClientHandler &MqttHandler = MqttClientHandler::getInstance();
		MqttHandler.setup_Mqtt();
	}
}

unsigned long last = 0;

void loop()
{

	unsigned long currentSeconds = millis() / 1000;

	BME680Handler &bmehandler = BME680Handler::getInstance();
    bmehandler.updateSensorData(currentSeconds);

    Bsec bme_data = bmehandler.getData(); 

	MHZ19Handler &mhz19Handler = MHZ19Handler::getInstance();
	mhz19Handler.runUpdate(currentSeconds);

	DataCO2 mhz19Readout = mhz19Handler.getLastReadout();

	if (configHandler.getConfigSwitch("switchWIFI"))
	{
		WiFiHandler::checkWifiStatus(currentSeconds);
	}

	WebServerHandler &webServer = WebServerHandler::getInstance();
	webServer.setInputDataforBody(mhz19Readout, bme_data, localTime("%Y.%m.%d %H:%M:%S"));

	EPDHandler &epdHandler = EPDHandler::getInstance();
    if (configHandler.getConfigSwitch("switchEPD"))
    {
        String wlan_ssid = WiFi.SSID();
        String ip_address = WiFi.localIP().toString();
        epdHandler.updateEPD(mhz19Readout, bme_data, localTime("%Y.%m.%d"), localTime("%H:%M"), wlan_ssid, ip_address, currentSeconds);
        epdHandler.clearPendingRefresh();
    }
    else
    {
        if (epdHandler.isPendingRefresh())
        {
            epdHandler.clearPendingRefresh();
            epdHandler.wipeDisplay();
        }
    }
	
	if (configHandler.getConfigSwitch("switchLED"))
	{
		LEDHandler &ledHandler = LEDHandler::getInstance();
		ledHandler.setInputDataforLED(mhz19Readout, bme_data);
		ledHandler.ledstatus(currentSeconds);
	}
	else
	{
		LEDHandler &ledHandler = LEDHandler::getInstance();
		ledHandler.setup_black(currentSeconds);
	}

	if (configHandler.getConfigSwitch("switchMQTT"))
	{
		MqttClientHandler &MqttHandler = MqttClientHandler::getInstance();
		if (!MqttHandler.isSetup())
		{
			MqttHandler.setup_Mqtt();
		}
		MqttHandler.publishData(mhz19Readout, bme_data, currentSeconds);
	}
}