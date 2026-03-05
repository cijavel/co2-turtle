#ifndef CO2_TURTLE_EPDHANDLER_H
#define CO2_TURTLE_EPDHANDLER_H

#include "DataCO2.h"
#include "bsec.h"
#include "BME680Handler.h"

class EPDHandler
{
public:
    static EPDHandler &getInstance()
    {
        static EPDHandler instance;
        return instance;
    }

    void updateEPD(const DataCO2 &co2, const Bsec &bme_data, const String &epd_date, const String &epd_time, unsigned long currentSeconds);
    void forceRefresh();

private:
    EPDHandler() {};
    EPDHandler(EPDHandler const &);
    void operator=(EPDHandler const &);

    void printVertically(const DataCO2 &co2, const Bsec &bme_data, const String &epd_date, const String &epd_time, bool bmeOk);
    void printHorizontally(const DataCO2 &co2, const Bsec &bme_data, const String &epd_date, const String &epd_time, bool bmeOk);
    void printSensorValues(const DataCO2 &co2, const Bsec &bme_data, bool bmeOk, int16_t col1, int16_t col2, int16_t rowTemp, int16_t rowHum, int16_t rowIaq, int16_t rowCo2);
    void printValue(char *buff, int16_t x, int16_t y, uint16_t color, float value);

    static uint16_t getAlertColor(float value, float threshold);
    static uint16_t getAlertColorInt(int value, int threshold);

    unsigned long _lastRunSeconds = 0;
};

#endif // CO2_TURTLE_EPDHANDLER_H