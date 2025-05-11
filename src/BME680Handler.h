#ifndef CO2_TURTLE_BME680HANDLER_H
#define CO2_TURTLE_BME680HANDLER_H

#include <Arduino.h>
#include <EEPROM.h>
#include "bsec.h"

class BME680Handler
{
public:
    static BME680Handler &getInstance()
    {
        static BME680Handler instance; 
        return instance;
    }
    Bsec getData();
    bool updateSensorData(const unsigned long currentSeconds);
    void printout() const;

private:
    void checkSensorStatus() const;
    void loadState(void);
    void updateState(void);
    [[noreturn]] static void executeLedError();
    Bsec bmeSensor;
    BME680Handler();
    BME680Handler(BME680Handler const &);
    void operator=(BME680Handler const &); 
    void updateSensorDataInternal();
};
#endif // CO2_TURTLE_BME680HANDLER_H
