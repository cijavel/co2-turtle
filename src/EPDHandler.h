#ifndef CO2_TURTLE_EPDHANDLER_H
#define CO2_TURTLE_EPDHANDLER_H

#include "DataCO2.h"
#include "bsec.h"
#include "BME680Handler.h"
#include <atomic>
class EPDHandler;  


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
    int rowTop2;
    int rowBot2;

    // Footer
    int footerTurtleX;
    int footerTurtleY;
    int footerWlanIconX;
    int footerWlanIconY;
    int footerWlanX;
    int footerWlanY;
    int footerIpX;
    int footerIpY;
    int footerDateX;
    int footerDateY;
    int footerNameX;
    int footerNameY;
    int footerMaxChars; 
};

struct EPDTaskParams
{
    EPDHandler *handler;
    DataCO2     co2;
    Bsec        bme_data;
    String      epd_date;
    String      epd_time;
    String      wlan_ssid;
    String      ip_address;
    bool        bmeOk;
    EPDLayout   layout;
};

struct EPDWipeParams
{
    EPDHandler *h;
    EPDLayout   l;
};

class EPDHandler
{
public:
    static EPDHandler &getInstance()
    {
        static EPDHandler instance;
        return instance;
    }

    void updateEPD(const DataCO2 &co2, const Bsec &bme_data, const String &epd_date, const String &epd_time, const String &wlan_ssid, const String &ip_address, unsigned long currentSeconds);
    void forceRefresh();
    void wipeDisplay();
    bool isPendingRefresh() const { return _pendingRefresh; }
    void clearPendingRefresh()    { _pendingRefresh = false; }
    

private:
    EPDHandler() {};
    EPDHandler(EPDHandler const &);
    void operator=(EPDHandler const &);

    void printValue(char *buff, int16_t x, int16_t y, uint16_t color, float value);
    void printLayout(const DataCO2 &co2, const Bsec &bme_data, const String &epd_date, const String &epd_time, const String &wlan_ssid, const String &ip_address, bool bmeOk, const EPDLayout &layout);

    static void epdRenderTask(void *pvParameters);
    static void epdWipeTask(void *pvParameters);

    static EPDLayout layout_vertical(int rotation);
    static EPDLayout layout_horizontal(int rotation);

    static uint16_t getAlertColor(float value, float threshold);

    std::atomic<bool> _taskRunning{false};
    unsigned long _lastRunSeconds = 0;
    bool _wiped = false;
    bool _pendingRefresh = false;
    bool _forceRender = false;
    
};

#endif // CO2_TURTLE_EPDHANDLER_H