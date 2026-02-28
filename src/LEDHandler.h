#ifndef CO2_TURTLE_LEDHANDLER_H
#define CO2_TURTLE_LEDHANDLER_H

#include "Configuration.h"
#include "DataCO2.h"
#include "bsec.h"
#include <WiFi.h>
#include <SPI.h>
#include <FastLED.h>
#include "LEDsection.h"

class LEDHandler
{
public:
	static LEDHandler &getInstance()
	{
		static LEDHandler instance; // Guaranteed to be destroyed.
		return instance;				// Instantiated on first use.
	}

	void setup_led();
	bool setup_black(const unsigned long currentSeconds);
	bool ledstatus(const unsigned long currentSeconds);
	void setInputDataforLED(DataCO2 co2Sensordata, Bsec enviromentdata);
	void updateLEDBrightness(int brightness);

private:
	void setSectionColor(SectionName sectionName, CRGB color);
	void ledStatusWiFi();
	void ledStatusBME();
	void ledStatusCO2();
	
	DataCO2 co2data;
	Bsec bmedata;
	LEDHandler() {};					// Constructor? (the {} brackets) are needed here.
	LEDHandler(LEDHandler const &); // Don't Implement
	void operator=(LEDHandler const &); // Don't implement
	unsigned long _lastRunSeconds = 0;
};
#endif // CO2_TURTLE_LEDHANDLER_H
