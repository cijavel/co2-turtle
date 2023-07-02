#include "Configuration.h"
#include "FastLedHandler.h"
#include <WiFi.h>
#include <helpers.h>

FASTLED_USING_NAMESPACE

#define MAX_POWER_MILLIAMPS 500
#define LED_TYPE  WS2812B
#define COLOR_ORDER GRB

#define LED_WLANCONNECT 0
#define LED_STATUS 1
#define LED_TEMP 2
#define LED_HUM 3
#define LED_AIRQ 4
#define LED_CO2 5

#define FASTLED_ALLOW_INTERRUPTS
#define BRIGHTNESS_LEDS 10
#define DATA_PIN 4
#define NUM_LEDS 34

CRGB led[NUM_LEDS];
SectionManager LEDsectionManager = SectionManager(led);
int ledloop = 0;



void FastLedHandler::addLEDsection()
{
    LEDsectionManager.addSections(6);
    LEDsectionManager.addRangeToSection(LED_WLANCONNECT, 0, 0, false); // LED_WLAN
    LEDsectionManager.addRangeToSection(LED_STATUS, 1, 1, false);      // LED_STATUS data Quality
    LEDsectionManager.addRangeToSection(LED_TEMP, 3, 9, false);        // LED_TEMP
    LEDsectionManager.addRangeToSection(LED_HUM, 11, 17, false);       // LED_HUM
    LEDsectionManager.addRangeToSection(LED_AIRQ, 19, 25, false);      // LED_AIRQ
    LEDsectionManager.addRangeToSection(LED_CO2, 27, 33, false);       // LED_CO2

}

void FastLedHandler::setup_led()
{
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(led, NUM_LEDS);
    FastLED.clear(true);
    FastLED.setCorrection( TypicalLEDStrip );
    FastLED.setMaxPowerInVoltsAndMilliamps( 5, MAX_POWER_MILLIAMPS);
    FastLED.setBrightness(BRIGHTNESS_LEDS);
}

uint16_t FastLedHandler::rainbowAllSections(uint8_t pauseDuration, uint16_t wheelPosition, uint16_t multi)
{
    if (NUM_LEDS * multi < wheelPosition)
    {
        wheelPosition = 0;
    }
    wheelPosition += 1;

    int colorsteps = 230; // how many colors, 256 all color,
    int colors = 9;       // circle. Distance between colors per cycle. the higher the finer
    uint16_t level;
    // for (level = 0; level > LEDsectionManager.getTotalLevels(); level++) // show against clockwise
    for (level = LEDsectionManager.getTotalLevels(); level > 0; level--) // show clockwise
    {
        uint32_t color = Wheel((level * colors + wheelPosition) & colorsteps);

        for (uint8_t i = LEDsectionManager.getTotalLevels(); i > 0; i--) // change color clockwise
        {
            LEDsectionManager.setColorAtGlobalIndex(level, color);
        }

        FastLED.setBrightness(50);
        FastLED.show();
        delay(pauseDuration);
    }
    return wheelPosition;
}

void FastLedHandler::fastLedWiFi()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        LEDsectionManager.fillSectionWithColor(LED_WLANCONNECT, CRGB::DarkRed, FillStyle(ALL_AT_ONCE));
        FastLED.show();
        delay(150);

        LEDsectionManager.fillSectionWithColor(LED_WLANCONNECT, CRGB::Black, FillStyle(ALL_AT_ONCE));
        FastLED.show();
        delay(1000);
    }
    else
    {
        LEDsectionManager.fillSectionWithColor(LED_WLANCONNECT, CRGB::LightSkyBlue, FillStyle(ALL_AT_ONCE));
        FastLED.show();
    }
    FastLED.show();
}

void FastLedHandler::fastLedBME()
{
    switch (bmedata.iaqAccuracy)
    {
    case 0:
    {
        // Calibration phase. Please wait....
        ledloop = rainbowAllSections(20, ledloop, 255);
        break;
    }
    case 1:
    {
        // learning
        LEDsectionManager.fillSectionWithColor(LED_STATUS, CRGB::Yellow, FillStyle(ALL_AT_ONCE));
        break;
    }
    case 2:
    {
        // good
        LEDsectionManager.fillSectionWithColor(LED_STATUS, CRGB::GreenYellow, FillStyle(ALL_AT_ONCE));
        break;
    }
    case 3:
    {
        // good. start saving them.
        LEDsectionManager.fillSectionWithColor(LED_STATUS, CRGB::Green, FillStyle(ALL_AT_ONCE));
        break;
    }
    default:
    {
        // this should never happen! iaqAccuracy < 0 || iaqAccuracy > 3");
    }
    }

    if (bmedata.temperature)
    {
        if (bmedata.temperature < 16) // too cold
        {
            LEDsectionManager.fillSectionWithColor(LED_TEMP, CRGB::LightBlue, FillStyle(ALL_AT_ONCE));
        }
        else if (bmedata.temperature < 18) // cold
        {
            LEDsectionManager.fillSectionWithColor(LED_TEMP, CRGB::Blue, FillStyle(ALL_AT_ONCE));
        }
        else if (bmedata.temperature < 20) // cool
        {
            LEDsectionManager.fillSectionWithColor(LED_TEMP, CRGB::SeaGreen, FillStyle(ALL_AT_ONCE));
        }
        else if (bmedata.temperature < 22) // normal
        {
            LEDsectionManager.fillSectionWithColor(LED_TEMP, CRGB::Green, FillStyle(ALL_AT_ONCE));
        }
        else if (bmedata.temperature < 24) // cosy
        {
            LEDsectionManager.fillSectionWithColor(LED_TEMP, CRGB::GreenYellow, FillStyle(ALL_AT_ONCE));
        }
        else if (bmedata.temperature < 26) // warm
        {
            LEDsectionManager.fillSectionWithColor(LED_TEMP, CRGB::Yellow, FillStyle(ALL_AT_ONCE));
        }
        else if (bmedata.temperature < 28) // hot
        {
            LEDsectionManager.fillSectionWithColor(LED_TEMP, CRGB::Orange, FillStyle(ALL_AT_ONCE));
        }
        else if (bmedata.temperature > 28) // scalding hot
        {
            LEDsectionManager.fillSectionWithColor(LED_TEMP, CRGB::Red, FillStyle(ALL_AT_ONCE));
        }
        else // way too hot
        {
            LEDsectionManager.fillSectionWithColor(LED_TEMP, CRGB::Magenta, FillStyle(ALL_AT_ONCE));
        }
    }

    if (bmedata.humidity)
    {
        if (bmedata.humidity < 20) // Far too dry
        {
            LEDsectionManager.fillSectionWithColor(LED_HUM, CRGB::Red, FillStyle(ALL_AT_ONCE));
        }
        else if (bmedata.humidity < 30) // Too dry
        {
            LEDsectionManager.fillSectionWithColor(LED_HUM, CRGB::Yellow, FillStyle(ALL_AT_ONCE));
        }
        else if (bmedata.humidity < 40) // dry
        {
            LEDsectionManager.fillSectionWithColor(LED_HUM, CRGB::GreenYellow, FillStyle(ALL_AT_ONCE));
        }
        else if (bmedata.humidity < 50) // normal
        {
            LEDsectionManager.fillSectionWithColor(LED_HUM, CRGB::Green, FillStyle(ALL_AT_ONCE));
        }
        else if (bmedata.humidity < 60) // Slightly moist
        {
            LEDsectionManager.fillSectionWithColor(LED_HUM, CRGB::YellowGreen, FillStyle(ALL_AT_ONCE));
        }
        else if (bmedata.humidity < 65) // moist
        {
            LEDsectionManager.fillSectionWithColor(LED_HUM, CRGB::Orange, FillStyle(ALL_AT_ONCE));
        }
        else if (bmedata.humidity < 70) // very moist
        {
            LEDsectionManager.fillSectionWithColor(LED_HUM, CRGB::Red, FillStyle(ALL_AT_ONCE));
        }
        else // wet
        {
            LEDsectionManager.fillSectionWithColor(LED_HUM, CRGB::Black, FillStyle(ALL_AT_ONCE));
            FastLED.show();
            delay(150);

            LEDsectionManager.fillSectionWithColor(LED_HUM, CRGB::Magenta, FillStyle(ALL_AT_ONCE));
            FastLED.show();
            delay(500);
        }
    }

    if (bmedata.iaq > 0)
    {
        if (bmedata.iaq <= 50) // exellent
        {
            LEDsectionManager.fillSectionWithColor(LED_AIRQ, CRGB::SeaGreen, FillStyle(ALL_AT_ONCE));
        }
        else if (bmedata.iaq <= 100) // good
        {
            LEDsectionManager.fillSectionWithColor(LED_AIRQ, CRGB::Green, FillStyle(ALL_AT_ONCE));
        }
        else if (bmedata.iaq <= 150) // lightly polluted. Ventilation suggested.
        {
            LEDsectionManager.fillSectionWithColor(LED_AIRQ, CRGB::YellowGreen, FillStyle(ALL_AT_ONCE));
        }
        else if (bmedata.iaq <= 200) // moderately polluted. please ventilate.
        {
            LEDsectionManager.fillSectionWithColor(LED_AIRQ, CRGB::Yellow, FillStyle(ALL_AT_ONCE));
        }
        else if (bmedata.iaq < 250) // heavily polluted. please ventilate.
        {
            LEDsectionManager.fillSectionWithColor(LED_AIRQ, CRGB::Orange, FillStyle(ALL_AT_ONCE));
        }
        else if (bmedata.iaq < 300) // severly polluted. please ventilate urgently.
        {
            LEDsectionManager.fillSectionWithColor(LED_AIRQ, CRGB::Red, FillStyle(ALL_AT_ONCE));
        }
        else // extremly polluted. please ventilate urgently.
        {
            LEDsectionManager.fillSectionWithColor(LED_AIRQ, CRGB::Black, FillStyle(ALL_AT_ONCE));
            FastLED.show();
            delay(150);

            LEDsectionManager.fillSectionWithColor(LED_AIRQ, CRGB::Magenta, FillStyle(ALL_AT_ONCE));
            FastLED.show();
            delay(500);
        }
    }
    FastLED.show();
}

void FastLedHandler::fastLedCO2()
{
    if (co2data.getRegular() > 0)
    {
        if (co2data.getRegular() < 600) // outdoor air
        {
            LEDsectionManager.fillSectionWithColor(LED_CO2, CRGB::Blue, FillStyle(ALL_AT_ONCE));
        }
        else if (co2data.getRegular() < 800) // fresh indoor air
        {
            LEDsectionManager.fillSectionWithColor(LED_CO2, CRGB::Green, FillStyle(ALL_AT_ONCE));
        }
        else if (co2data.getRegular() < 1000) // Indoor air
        {
            LEDsectionManager.fillSectionWithColor(LED_CO2, CRGB::GreenYellow, FillStyle(ALL_AT_ONCE));
        }
        else if (co2data.getRegular() < 1200) // used indoor air. please ventilate
        {
            LEDsectionManager.fillSectionWithColor(LED_CO2, CRGB::Yellow, FillStyle(ALL_AT_ONCE));
        }
        else if (co2data.getRegular() < 1400) // stale indoor air. please ventilate
        {
            LEDsectionManager.fillSectionWithColor(LED_CO2, CRGB::Orange, FillStyle(ALL_AT_ONCE));
        }
        else if (co2data.getRegular() < 1600) // strongly stale indoor air. please ventilate urgently. thinking performance impaired
        {
            LEDsectionManager.fillSectionWithColor(LED_CO2, CRGB::Red, FillStyle(ALL_AT_ONCE));
        }
        else // Tiredness, headache. please ventilate urgently.
        {
            LEDsectionManager.fillSectionWithColor(LED_CO2, CRGB::Black, FillStyle(ALL_AT_ONCE));
            FastLED.show();
            delay(150);

            LEDsectionManager.fillSectionWithColor(LED_CO2, CRGB::Magenta, FillStyle(ALL_AT_ONCE));
            FastLED.show();
            delay(500);
        }
    }
    FastLED.show();
}

void FastLedHandler::fastinit()
{
  if (bmedata.iaqAccuracy == 0)
  {
    ledloop = rainbowAllSections(20, ledloop, 255);
  }
    FastLED.show();
}
