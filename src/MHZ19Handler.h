#ifndef CO2_TURTLE_MHZ19HANDLER_H
#define CO2_TURTLE_MHZ19HANDLER_H
#include <SoftwareSerial.h>
#include "MHZ19.h"
#include "Arduino.h"
#include "Configuration.h"
#include "CO2Data.h"

class MHZ19Handler {
public:
    static MHZ19Handler &getInstance() {
        static MHZ19Handler instance; // Guaranteed to be destroyed.
        return instance;// Instantiated on first use.
    }
    void printoutCurrentValues();
    void printoutLastReadout();
    CO2Data getLastReadout();
    bool runUpdate(unsigned long currentSeconds);
private:
    bool updateLastReadout();
    CO2Data _lastReadout;
    MHZ19 myMHZ19;
    SoftwareSerial* Serial_MHZ19;
    MHZ19Handler();                    // Constructor? (the {} brackets) are needed here.
    MHZ19Handler(MHZ19Handler const&);  // Don't Implement
    void operator=(MHZ19Handler const&); // Don't implement
};
#endif //CO2_TURTLE_MHZ19HANDLER_H
