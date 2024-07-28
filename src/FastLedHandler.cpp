#include "Configuration.h"
#include "FastLedHandler.h"
#include "sectionLed.h"

CRGB leds[NUM_LEDS];

// Map section names to their corresponding sections
SectionStruc sections[NUM_SECTIONS] = {
    {0, 0},   // LED_WLANCONNECT
    {1, 1},   // LED_STATUS
    {3, 9},   // LED_TEMP
    {11, 17}, // LED_HUM
    {19, 25}, // LED_AIRQ
    {27, 33}  // LED_CO2
};

void FastLedHandler::setSectionColor(SectionName sectionName, CRGB color) {
    SectionStruc section = sections[sectionName];
    for (int i = section.startLed; i <= section.endLed; i++) {
        leds[i] = color;
    }
    FastLED.show();
}
void FastLedHandler::setup_led()
{
    FastLED.addLeds<LED_TYPE, DATA_LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.clear(true);
    FastLED.setCorrection( TypicalLEDStrip );
    FastLED.setMaxPowerInVoltsAndMilliamps( 5, MAX_POWER_MILLIAMPS);
    FastLED.setBrightness(BRIGHTNESS_LEDS);

    #ifdef DEBUG
            Serial.println("[FASTLED] Set LED configuration");
    #endif
}

void FastLedHandler::setInputDataforLED(DataCO2 co2Sensordata, Bsec enviromentdata)
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
        setSectionColor(LED_WLANCONNECT, CRGB::DarkRed);
        delay(150);

        setSectionColor(LED_WLANCONNECT, CRGB::Black);
        delay(150);
    }
    else
    {
        setSectionColor(LED_WLANCONNECT, CRGB::LightSkyBlue);
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
            setSectionColor(LED_STATUS, CRGB::Red);
            break;
        }
        case 1:
        {
            // learning
            setSectionColor(LED_STATUS, CRGB::Yellow);
            break;
        }
        case 2:
        {
            // good
            setSectionColor(LED_STATUS, CRGB::GreenYellow);
            break;
        }
        case 3:
        {
            // good. start saving them.
            setSectionColor(LED_STATUS, CRGB::Green);
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
            setSectionColor(LED_TEMP, CRGB::LightBlue);
        }
        else if (bmedata.temperature < 18) // cold
        {
            setSectionColor(LED_TEMP, CRGB::Blue);
        }
        else if (bmedata.temperature < 20) // cool
        {
            setSectionColor(LED_TEMP, CRGB::SeaGreen);
        }
        else if (bmedata.temperature < 22) // normal
        {
            setSectionColor(LED_TEMP, CRGB::Green);
        }
        else if (bmedata.temperature < 24) // cosy
        {
            setSectionColor(LED_TEMP, CRGB::GreenYellow);
        }
        else if (bmedata.temperature < 26) // warm
        {
            setSectionColor(LED_TEMP, CRGB::Yellow);
        }
        else if (bmedata.temperature < 28) // hot
        {
            setSectionColor(LED_TEMP, CRGB::Orange);
        }
        else if (bmedata.temperature > 28) // scalding hot
        {
            setSectionColor(LED_TEMP, CRGB::Red);
        }
        else // way too hot
        {
            setSectionColor(LED_TEMP, CRGB::Magenta);
        }
    }

    if (bmedata.humidity)
    {
        if (bmedata.humidity < 20) // Far too dry
        {
            setSectionColor(LED_HUM, CRGB::Red);
        }
        else if (bmedata.humidity < 30) // Too dry
        {
            setSectionColor(LED_HUM, CRGB::Yellow);
        }
        else if (bmedata.humidity < 40) // dry
        {
            setSectionColor(LED_HUM, CRGB::GreenYellow);
        }
        else if (bmedata.humidity < 50) // normal
        {
            setSectionColor(LED_HUM, CRGB::Green);
        }
        else if (bmedata.humidity < 60) // Slightly moist
        {
            setSectionColor(LED_HUM, CRGB::YellowGreen);
        }
        else if (bmedata.humidity < 65) // moist
        {
            setSectionColor(LED_HUM, CRGB::Orange);
        }
        else if (bmedata.humidity < 70) // very moist
        {
            setSectionColor(LED_HUM, CRGB::Red);
        }
        else // wet
        {
            setSectionColor(LED_HUM, CRGB::Black);
            delay(150);

            setSectionColor(LED_HUM, CRGB::Magenta);
            delay(500);
        }
    }

    if (bmedata.iaq > 0)
    {
        if (bmedata.iaq <= 50) // exellent
        {
            setSectionColor(LED_AIRQ, CRGB::SeaGreen);
        }
        else if (bmedata.iaq <= 100) // good
        {
            setSectionColor(LED_AIRQ, CRGB::Green);
        }
        else if (bmedata.iaq <= 150) // lightly polluted. Ventilation suggested.
        {
            setSectionColor(LED_AIRQ, CRGB::YellowGreen);
        }
        else if (bmedata.iaq <= 200) // moderately polluted. please ventilate.
        {
            setSectionColor(LED_AIRQ, CRGB::Yellow);
        }
        else if (bmedata.iaq < 250) // heavily polluted. please ventilate.
        {
            setSectionColor(LED_AIRQ, CRGB::Orange);
        }
        else if (bmedata.iaq < 300) // severly polluted. please ventilate urgently.
        {
            setSectionColor(LED_AIRQ, CRGB::Red);
        }
        else // extremly polluted. please ventilate urgently.
        {
            setSectionColor(LED_AIRQ, CRGB::Black);
            delay(150);

            setSectionColor(LED_AIRQ, CRGB::Magenta);
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
            setSectionColor(LED_CO2, CRGB::Blue);
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
        else // Tiredness, headache. please ventilate urgently.
        {
            setSectionColor(LED_CO2, CRGB::Black);
            delay(150);

            setSectionColor(LED_CO2, CRGB::Magenta);
            delay(150);
        }
    }
    #ifdef DEBUG
            Serial.print("[FASTLED] co2data: ");
            Serial.println(String(co2data.getRegular()));
    #endif
    FastLED.show();
}

bool FastLedHandler::setup_black(const unsigned long currentSeconds)
{

    if (currentSeconds % interval_LED_in_Seconds == 0){
        FastLED.addLeds<LED_TYPE, DATA_LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
        FastLED.clear(true);
        FastLED.setBrightness(BRIGHTNESS_LEDS);
        FastLED.setCorrection( TypicalLEDStrip );
        FastLED.setMaxPowerInVoltsAndMilliamps( 5, MAX_POWER_MILLIAMPS);       
        Serial.println("[FASTLED] Set LED configuration to black");
        return true;
    }
    return false;
}