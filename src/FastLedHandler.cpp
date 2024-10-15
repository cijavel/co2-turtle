#include "Configuration.h"
#include "FastLedHandler.h"
#include "sectionLed.h"

CRGB leds[NUM_LEDS];

// Map section names to their corresponding sections
SectionStruc sections[NUM_SECTIONS] = {
    {0, 6},   // LED_TEMP
    {8, 14},  // LED_HUM
    {16, 17}, // LED_WLANCONNECT
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
}

void FastLedHandler::ledStatusBME()
{
    long temperature = bmedata.temperature+TEMPERATUR_OFFSET;
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

    if (bmedata.pressure > 0)
    {
        long pressure = bmedata.pressure/100;
        if (pressure <= 400) // wet
        {
            setSectionColor(LED_PRES, CRGB::RoyalBlue);
        }
        else if (pressure <= 500) // good
        {
            setSectionColor(LED_PRES, CRGB::Turquoise);
        }
        else if (pressure <= 600) // 
        {
            setSectionColor(LED_PRES, CRGB::Aquamarine);
        }
        else if (pressure <= 700) // 
        {
            setSectionColor(LED_PRES, CRGB::SpringGreen);
        }
        else if (pressure <= 800) // 
        {
            setSectionColor(LED_PRES, CRGB::Green);
        }
        else if (pressure < 900) // 
        {
            setSectionColor(LED_PRES, CRGB::GreenYellow);
        }
        else if (pressure < 1000) // 
        {
            setSectionColor(LED_PRES, CRGB::YellowGreen);
        }
        else // 
        {
            setSectionColor(LED_PRES, CRGB::Black);
            delay(150);

            setSectionColor(LED_PRES, CRGB::Gold);
            delay(500);
        }
    }
    #ifdef DEBUG
            Serial.print("[FASTLED] temperature: ");
            Serial.println(String(bmedata.temperature));
            Serial.print("[FASTLED] humidity: ");
            Serial.println(String(bmedata.humidity));
            Serial.print("[FASTLED] pressure: ");
            Serial.println(String(bmedata.pressure/100));
    #endif
}

void FastLedHandler::ledStatusCO2()
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