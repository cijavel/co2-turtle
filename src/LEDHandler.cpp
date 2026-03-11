#include "Configuration.h"
#include "LEDHandler.h"
#include "LEDsection.h"
#include "ConfigHandler.h"
#include "BME680Handler.h"
extern ConfigHandler &configHandler;

CRGB leds[NUM_LEDS];

// Map section names to their corresponding sections
SectionStruc sections[NUM_SECTIONS] = {
	//{0, 6},	  // LED_TEMP
	{0, 16},  // LED_HUM
	{18, 18}, // LED_WLANCONNECT
	{19, 19}, // LED_SENSORSTATE
	{21, 37}  // LED_CO2
};

void LEDHandler::setSectionColor(SectionName sectionName, CRGB color)
{
	SectionStruc section = sections[sectionName];
	for (int i = section.startLed; i <= section.endLed; i++)
	{
		leds[i] = color;
	}
	FastLED.show();
}

void LEDHandler::setup_led()
{
	FastLED.addLeds<LED_TYPE, DATA_LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
	FastLED.clear(true);
	FastLED.setCorrection(TypicalLEDStrip);
	FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_POWER_MILLIAMPS);
	FastLED.setBrightness(configHandler.getConfigLED("LEDbrightness"));
}

void LEDHandler::updateLEDBrightness(int brightness)
{
    FastLED.setBrightness(brightness);
    FastLED.show(); 
}

void LEDHandler::setInputDataforLED(DataCO2 co2Sensordata, Bsec enviromentdata)
{
	this->bmedata = enviromentdata;
	this->co2data = co2Sensordata;
}

bool LEDHandler::ledstatus(const unsigned long currentSeconds)
{
	if (currentSeconds - _lastRunSeconds >= (unsigned long)configHandler.getConfigInterval("intervalLED"))
	{
		_lastRunSeconds = currentSeconds;
		ledStatusWiFi();
		ledStatusBME();
		ledStatusCO2();
		return true;
	}
	return false;
}

void LEDHandler::ledStatusWiFi()
{
	if (WiFi.status() != WL_CONNECTED)
	{
		setSectionColor(LED_WLANCONNECT, CRGB::DarkRed);
		delay(150);

		setSectionColor(LED_WLANCONNECT, CRGB::Black);
		delay(150);
	}
	else
	{
		setSectionColor(LED_WLANCONNECT, CRGB::LightSkyBlue);
	}
}

void LEDHandler::ledStatusBME()
{
	long temperature = bmedata.temperature + TEMPERATUR_OFFSET;
	if (temperature)
	{
		if (temperature < 12) // colder
		{
			setSectionColor(LED_TEMP, CRGB::BlueViolet);
		}
		else if (temperature < 14) // colder
		{
			setSectionColor(LED_TEMP, CRGB::DarkBlue);
		}
		else if (temperature < 16) // colder
		{
			setSectionColor(LED_TEMP, CRGB::Blue);
		}
		else if (temperature < 18) // cold
		{
			setSectionColor(LED_TEMP, CRGB::LightBlue);
		}
		else if (temperature < 20) // cool
		{
			setSectionColor(LED_TEMP, CRGB::SeaGreen);
		}
		else if (temperature < 22) // normal
		{
			setSectionColor(LED_TEMP, CRGB::Green);
		}
		else if (temperature < 24) // cosy
		{
			setSectionColor(LED_TEMP, CRGB::GreenYellow);
		}
		else if (temperature < 26) // warm
		{
			setSectionColor(LED_TEMP, CRGB::Yellow);
		}
		else if (temperature < 28) // hot
		{
			setSectionColor(LED_TEMP, CRGB::Orange);
		}
		else if (temperature > 28) // scalding hot
		{
			setSectionColor(LED_TEMP, CRGB::Red);
		}
		else // way too hot
		{
			setSectionColor(LED_TEMP, CRGB::Magenta);
		}
	}

	if (!BME680Handler::getInstance().isSensorOk())
    {
        if (BME680Handler::getInstance().isRecovering())
            setSectionColor(LED_SENSORSTATE, CRGB::Orange);
        else
            setSectionColor(LED_SENSORSTATE, CRGB::Red);
        return;
    }
    setSectionColor(LED_SENSORSTATE, CRGB::Green);
}

void LEDHandler::ledStatusCO2()
{
	if (co2data.getRegular() > 0)
	{
		if (co2data.getRegular() < 500) // outdoor air
		{
			setSectionColor(LED_CO2, CRGB::Blue);
		}
		else if (co2data.getRegular() < 600) // fresh indoor air
		{
			setSectionColor(LED_CO2, CRGB::Turquoise);
		}
		else if (co2data.getRegular() < 800) // fresh indoor air
		{
			setSectionColor(LED_CO2, CRGB::Green);
		}
		else if (co2data.getRegular() < 1000) // Indoor air
		{
			setSectionColor(LED_CO2, CRGB::GreenYellow);
		}
		else if (co2data.getRegular() < 1200) // used indoor air. please ventilate
		{
			setSectionColor(LED_CO2, CRGB::Yellow);
		}
		else if (co2data.getRegular() < 1400) // stale indoor air. please ventilate
		{
			setSectionColor(LED_CO2, CRGB::Orange);
		}
		else if (co2data.getRegular() < 1600) // strongly stale indoor air. please ventilate urgently. thinking performance impaired
		{
			setSectionColor(LED_CO2, CRGB::Red);
		}
		else
		{
			setSectionColor(LED_CO2, CRGB::Black);
			delay(150);

			setSectionColor(LED_CO2, CRGB::Magenta);
			delay(150);
		}
	}
}

bool LEDHandler::setup_black(const unsigned long currentSeconds)
{
	if (currentSeconds - _lastRunSeconds >= (unsigned long)configHandler.getConfigInterval("intervalLED"))
	{
		_lastRunSeconds = currentSeconds;
		FastLED.addLeds<LED_TYPE, DATA_LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
		FastLED.clear(true);
		FastLED.setBrightness(BRIGHTNESS_LEDS);
		FastLED.setCorrection(TypicalLEDStrip);
		FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_POWER_MILLIAMPS);
		return true;
	}
	return false;
}