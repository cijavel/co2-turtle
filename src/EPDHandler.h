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
    static void printVertically(CO2Data co2, Bsec bsec, const String& epd_date, const String& epd_time);
private:
    EPDHandler() {};                    // Constructor? (the {} brackets) are needed here.
    EPDHandler(EPDHandler const&);  // Don't Implement
    void operator=(EPDHandler const&); // Don't implement
};
#endif //CO2_TURTLE_EPDHANDLER_H
