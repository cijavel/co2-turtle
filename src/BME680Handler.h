#ifndef CO2_TURTLE_BME680HANDLER_H
#define CO2_TURTLE_BME680HANDLER_H

#include <Arduino.h>
#include "bsec.h"

class BME680Handler {
public:
    static BME680Handler &getInstance() {
        static BME680Handler instance; // Guaranteed to be destroyed.
        return instance;// Instantiated on first use.
    }
    Bsec getData();
    bool updateSensorData();
    void printout() const;
private:
    void checkSensorStatus() const;
    [[noreturn]] static void executeLedError();
    Bsec data;
    BME680Handler();                    // Constructor? (the {} brackets) are needed here.
    BME680Handler(BME680Handler const&);  // Don't Implement
    void operator=(BME680Handler const&); // Don't implement
    void updateSensorDataInternal();
};
#endif //CO2_TURTLE_BME680HANDLER_H
