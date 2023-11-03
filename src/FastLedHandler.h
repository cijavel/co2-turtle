#ifndef CO2_TURTLE_FASTLEDHANDLER_H
#define CO2_TURTLE_FASTLEDHANDLER_H

#include "Configuration.h"
#include "CO2Data.h"
#include "bsec.h"

#include <WiFi.h>
#include <SPI.h>
#include <FastLED.h>
#include <SectionManager.h>

class FastLedHandler{
public:
    static FastLedHandler &getInstance() {
        static FastLedHandler instance; // Guaranteed to be destroyed.
        return instance;// Instantiated on first use.
    }

    void addLEDsection();
    void setup_led();
    bool ledstatus(const unsigned long currentSeconds);
    void setInputDataforLED(CO2Data co2Sensordata, Bsec enviromentdata);


private:
    void ledStatusWiFi();
    void ledStatusBME();
    void ledStatusCO2();
    
    CO2Data co2data;
    Bsec bmedata;
    FastLedHandler() {};                    // Constructor? (the {} brackets) are needed here.
    FastLedHandler(FastLedHandler const&);  // Don't Implement
    void operator=(FastLedHandler const&); // Don't implement
};
#endif //CO2_TURTLE_FASTLEDHANDLER_H
