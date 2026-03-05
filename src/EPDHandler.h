#ifndef CO2_TURTLE_EPDHANDLER_H
#define CO2_TURTLE_EPDHANDLER_H

#include "DataCO2.h"
#include "bsec.h"
#include "BME680Handler.h"

struct EPDLayout
{
    int rotation;

    // Sensor value columns
    int colL_icon;
    int colL_value;
    int colL_unit;
    int colR_icon;
    int colR_value;
    int colR_unit;

    // Sensor rows
    int rowTop;
    int rowBot;

    // Footer
    int footerTurtleX;
    int footerTurtleY;
    int footerDateX;
    int footerDateY;
    int footerTimeX;
    int footerTimeY;
    int footerNameX;
    int footerNameY;
};

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
    void clearDisplay();

private:
    EPDHandler() {};
    EPDHandler(EPDHandler const &);
    void operator=(EPDHandler const &);

    void printLayout(const DataCO2 &co2, const Bsec &bme_data, const String &epd_date, const String &epd_time, bool bmeOk, const EPDLayout &layout);
    void printValue(char *buff, int16_t x, int16_t y, uint16_t color, float value);

    static EPDLayout verticalLayout();
    static EPDLayout horizontalLayout();
    static uint16_t getAlertColor(float value, float threshold);
    static uint16_t getAlertColorInt(int value, int threshold);

    unsigned long _lastRunSeconds = 0;
};

#endif // CO2_TURTLE_EPDHANDLER_H