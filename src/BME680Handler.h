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
    bool isSensorOk() const;
    int getSensorError() const;

private:
    void checkSensorStatus() const;
    void loadState(void);
    void updateState(void);
    Bsec bmeSensor;
    BME680Handler();
    BME680Handler(BME680Handler const &);
    void operator=(BME680Handler const &); 
    unsigned long _lastRunSeconds = 0;
    unsigned long _lastRecoverySeconds = 0;
    bool _sensorOk = false;
    int _bme68xError = 0;
    int _consecutiveErrors = 0;
    bsec_virtual_sensor_t _sensorList[13];
};
#endif // CO2_TURTLE_BME680HANDLER_H
