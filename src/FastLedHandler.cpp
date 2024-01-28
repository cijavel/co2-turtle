#include "Configuration.h"
#include "FastLedHandler.h"
#include <helpers.h>

//#define FASTLED_RMT_MAX_CHANNELS 1
//#define FASTLED_ALL_PINS_HARDWARE_SPI
//#define FASTLED_ALLOW_INTERRUPTS


#define LED_WLANCONNECT 0
#define LED_STATUS 1
#define LED_TEMP 2
#define LED_HUM 3
#define LED_AIRQ 4
#define LED_CO2 5


CRGB led[NUM_LEDS];
SectionManager LEDsectionManager = SectionManager(led);


void FastLedHandler::addLEDsection()
{
    LEDsectionManager.addSections(6);
    LEDsectionManager.addRangeToSection(LED_WLANCONNECT, 0, 0, false); // LED_WLAN
    LEDsectionManager.addRangeToSection(LED_STATUS, 1, 1, false);      // LED_STATUS data Quality
    LEDsectionManager.addRangeToSection(LED_TEMP, 3, 9, false);        // LED_TEMP
    LEDsectionManager.addRangeToSection(LED_HUM, 11, 17, false);       // LED_HUM
    LEDsectionManager.addRangeToSection(LED_AIRQ, 19, 25, false);      // LED_AIRQ
    LEDsectionManager.addRangeToSection(LED_CO2, 27, 33, false);       // LED_CO2
    #ifdef DEBUG
            Serial.println("[FASTLED] Set LED sections");
    #endif
}

void FastLedHandler::setup_led()
{
    FastLED.addLeds<LED_TYPE, DATA_LED_PIN, COLOR_ORDER>(led, NUM_LEDS);
    FastLED.clear(true);
    FastLED.setCorrection( TypicalLEDStrip );
    FastLED.setMaxPowerInVoltsAndMilliamps( 5, MAX_POWER_MILLIAMPS);
    FastLED.setBrightness(BRIGHTNESS_LEDS);

    #ifdef DEBUG
            Serial.println("[FASTLED] Set LED configuration");
    #endif
}

void FastLedHandler::setCo2AndData(CO2Data co2Sensordata, Bsec enviromentdata)
{
  this->bmedata = enviromentdata;
  this->co2data = co2Sensordata;
}

bool FastLedHandler::ledstatus(const unsigned long currentSeconds)
{
    if (currentSeconds % interval_LED_in_Seconds == 0){
        ledStatusWiFi();
        ledStatusBME();
        ledStatusCO2();
        return true;
    }
    return false;
}

void FastLedHandler::ledStatusWiFi()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        LEDsectionManager.fillSectionWithColor(LED_WLANCONNECT, CRGB::DarkRed, FillStyle(ALL_AT_ONCE));
        FastLED.show();
        delay(150);

        LEDsectionManager.fillSectionWithColor(LED_WLANCONNECT, CRGB::Black, FillStyle(ALL_AT_ONCE));
        FastLED.show();
        delay(150);
    }
    else
    {
        LEDsectionManager.fillSectionWithColor(LED_WLANCONNECT, CRGB::LightSkyBlue, FillStyle(ALL_AT_ONCE));
        FastLED.show();
    }
    #ifdef DEBUG
            Serial.println("[FASTLED] before show WIFI");
    #endif
    FastLED.show();
}

void FastLedHandler::ledStatusBME()
{
    switch (bmedata.iaqAccuracy)
    {
        case 0:
        {
            // Calibration phase. Please wait....
            LEDsectionManager.fillSectionWithColor(LED_STATUS, CRGB::Red, FillStyle(ALL_AT_ONCE));
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
    #ifdef DEBUG
            Serial.print("[FASTLED] iaqAccuracy: ");
            Serial.println(String(bmedata.iaqAccuracy));
            Serial.print("[FASTLED] temperature: ");
            Serial.println(String(bmedata.temperature));
            Serial.print("[FASTLED] humidity: ");
            Serial.println(String(bmedata.humidity));
            Serial.print("[FASTLED] iaq: ");
            Serial.println(String(bmedata.iaq));
    #endif
    FastLED.show();
}

void FastLedHandler::ledStatusCO2()
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
            delay(150);
        }
    }
    #ifdef DEBUG
            Serial.print("[FASTLED] co2data: ");
            Serial.println(String(co2data.getRegular()));
    #endif
    FastLED.show();
}

