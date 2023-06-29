#ifndef CO2_TURTLE_FASTLEDHANDLER_H
#define CO2_TURTLE_FASTLEDHANDLER_H

#include "Configuration.h"
#include "CO2Data.h"
#include "bsec.h"

#include <FastLED.h>
#include <SectionManager.h>
#include <helpers.h>

class FastLedHandler{
public:
    static FastLedHandler &getInstance() {
        static FastLedHandler instance; // Guaranteed to be destroyed.
        return instance;// Instantiated on first use.
    }
    void fastLedWiFi();
    void fastLedBME();
    void fastLedCO2();
    void addLEDsection();

private:
    int rainbowAllSections(uint8_t pauseDuration, uint16_t wheelPosition, int multi);
    CO2Data co2data;
    Bsec bmedata;
    FastLedHandler() {};                    // Constructor? (the {} brackets) are needed here.
    FastLedHandler(FastLedHandler const&);  // Don't Implement
    void operator=(FastLedHandler const&); // Don't implement
};
#endif //CO2_TURTLE_FASTLEDHANDLER_H
