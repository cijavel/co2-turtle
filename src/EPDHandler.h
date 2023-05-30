#ifndef CO2_TURTLE_EPDHANDLER_H
#define CO2_TURTLE_EPDHANDLER_H

#include "CO2Data.h"
#include "bsec.h"

class EPDHandler{
public:
    static EPDHandler &getInstance() {
        static EPDHandler instance; // Guaranteed to be destroyed.
        return instance;// Instantiated on first use.
    }
    static void updateEPDvertical(CO2Data co2, Bsec data, const String& epd_date, const String& epd_time, unsigned long currentSeconds);
    static void updateEPDhorizontal(CO2Data co2, Bsec data, const String& epd_date, const String& epd_time, unsigned long currentSeconds);
private:
    static void printVertically(CO2Data co2, Bsec bme_data, const String& epd_date, const String& epd_time);
    static void printHorizontally(CO2Data co2, Bsec bme_data, const String& epd_date, const String& epd_time);
    EPDHandler() {};                    // Constructor? (the {} brackets) are needed here.
    EPDHandler(EPDHandler const&);  // Don't Implement
    void operator=(EPDHandler const&); // Don't implement
    static void PrintEspLine(char *buff, int16_t cursorX, int16_t cursorY, uint16_t color, float toPrint);
};
#endif //CO2_TURTLE_EPDHANDLER_H
