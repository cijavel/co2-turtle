#include "EPDHandler.h"
#include "ConfigHandler.h"
#include <GxEPD2_3C.h>
#include "GxEPD2_display_selection_new_style.h"
#include "../font/BabelSans8pt7b.h"
#include "../font/BabelSans9pt7b.h"
#include "../font/BabelSans10pt7b.h"
#include "../font/Inter_Regular10pt7b.h"
#include "../font/Inter_Regular8pt7b.h"
#include "../font/Inter_Bold12pt7b.h"
#include "Configuration.h"
#include "symbol.h"
#include <WiFi.h>

extern ConfigHandler &configHandler;

#define EPD_FLOAT_BUFFER_SIZE 8  // enough for "-123.4\0"

uint16_t EPDHandler::getAlertColor(float value, float threshold)
{
    return (value >= threshold) ? GxEPD_RED : GxEPD_BLACK;
}


void EPDHandler::printValue(char *buff, int16_t x, int16_t y, uint16_t color, float value)
{
    display.setTextColor(color);
    display.setCursor(x, y);
    snprintf(buff, EPD_FLOAT_BUFFER_SIZE, "%.1f", value);
    display.print(buff);
}

EPDLayout EPDHandler::layout_vertical(int rotation)
{
    EPDLayout l;
    l.rotation  = rotation;  // 2 = V90, 0 = V270

    l.colL_icon = 2;           l.colL_value = 30;           l.colL_unit = 101;
    l.colR_icon = l.colL_icon; l.colR_value = l.colL_value; l.colR_unit = l.colL_unit;

    l.rowTop    = 30;
    l.rowBot    = 68;
    l.rowTop2   = 106;
    l.rowBot2   = 144;

    l.footerWlanIconX = 1;  l.footerWlanIconY = 176;
    l.footerWlanX     = 20; l.footerWlanY     = 191;
    l.footerIpX       = 1;  l.footerIpY       = 209;
    l.footerDateX     = 1;  l.footerDateY     = 227;
    l.footerTurtleX   = 1;  l.footerTurtleY   = 230;
    l.footerNameX     = 20; l.footerNameY     = 245;

    l.footerMaxChars  = 16;
    return l;
}

EPDLayout EPDHandler::layout_horizontal(int rotation)
{
    EPDLayout l;
    l.rotation  = rotation;  // 1 = H90, 3 = H270

    l.colL_icon = 0;   l.colL_value = 30;  l.colL_unit = 90;
    l.colR_icon = 130; l.colR_value = 160; l.colR_unit = 230;

    l.rowTop  = 20; l.rowTop2 = 20;
    l.rowBot  = 54; l.rowBot2 = 54;

    l.footerWlanIconX = 2;   l.footerWlanIconY = 70;
    l.footerWlanX     = 20;  l.footerWlanY     = 88;
    l.footerIpX       = 120; l.footerIpY       = 102;
    l.footerDateX     = 2;   l.footerDateY     = 102;
    l.footerTurtleX   = 2;   l.footerTurtleY   = 104;
    l.footerNameX     = 20;  l.footerNameY     = 120;

    l.footerMaxChars  = 30;
    return l;
}

void EPDHandler::epdRenderTask(void *pvParameters)
{
    EPDTaskParams *p = static_cast<EPDTaskParams *>(pvParameters);
    p->handler->printLayout(p->co2, p->bme_data, p->epd_date, p->epd_time, p->wlan_ssid, p->ip_address, p->bmeOk, p->layout);
    p->handler->_taskRunning = false;
    delete p;
    vTaskDelete(nullptr);
}

void EPDHandler::printLayout(const DataCO2 &co2, const Bsec &bme_data, const String &epd_date, const String &epd_time, const String &wlan_ssid, const String &ip_address, bool bmeOk, const EPDLayout &l)
{
    display.init(BAUDRATE);
    display.fillScreen(GxEPD_WHITE);
    display.setFullWindow();
    display.setRotation(l.rotation);

    float temperature = bme_data.temperature + configHandler.getConfigSensor("tempOffset") / 10.0f;

    uint16_t color_temp = getAlertColor(temperature, 26);
    uint16_t color_hum  = getAlertColor(bme_data.humidity, 70);
    uint16_t color_iaq = getAlertColor(bme_data.iaq, 300);
    uint16_t color_co2 = getAlertColor(co2.getRegular(), 1500);

    char buffer[EPD_FLOAT_BUFFER_SIZE];
    display.setFont(&Inter_Bold12pt7b);

    // Left column icons
    display.drawInvertedBitmap(l.colL_icon, l.rowTop - 20, bitmap_temp, 24, 24, color_temp);
    display.drawInvertedBitmap(l.colL_icon, l.rowBot - 20, bitmap_hum,  24, 24, color_hum);

    // Left column values: Temperature (top), Humidity (bottom)
    if (bmeOk)
    {
        printValue(buffer, l.colL_value, l.rowTop, color_temp, temperature);
        display.drawInvertedBitmap(l.colL_icon, l.rowBot - 20, bitmap_hum,  24, 24, color_hum);
    }

    // Left column values: Temperature (top), Humidity (bottom)
    if (bmeOk)
    {
        printValue(buffer, l.colL_value, l.rowTop, color_temp, bme_data.temperature);
        printValue(buffer, l.colL_value, l.rowBot, color_hum,  bme_data.humidity);
    }
    else
    {
        display.setTextColor(GxEPD_BLACK);
        display.setCursor(l.colL_value, l.rowTop); display.print("--");
        display.setCursor(l.colL_value, l.rowBot); display.print("--");
    }

    // Left column units
    display.drawInvertedBitmap(l.colL_unit, l.rowTop - 16, bitmap_grad18, 18, 18, color_temp);
    display.setFont(&Inter_Regular10pt7b);
    display.setTextColor(color_hum);
    display.setCursor(l.colL_unit, l.rowBot);
    display.print("%");

    // Right column icons
    display.drawInvertedBitmap(l.colR_icon, l.rowTop2 - 20, bitmap_CO2, 24, 24, color_co2);
    display.drawInvertedBitmap(l.colR_icon, l.rowBot2 - 20, bitmap_aiq, 24, 24, color_iaq);

    // Right column values: CO2 (top), IAQ (bottom)
    display.setFont(&Inter_Bold12pt7b);
    display.setTextColor(color_co2);
    display.setCursor(l.colR_value, l.rowTop2);
    snprintf(buffer, sizeof(buffer), "%d", co2.getRegular());
    display.print(buffer);
    display.drawInvertedBitmap(l.colR_unit, l.rowTop2 - 16, bitmap_ppm18, 18, 18, color_co2);
    display.drawInvertedBitmap(l.colR_unit, l.rowBot2 - 16, bitmap_iaq,   18, 18, color_iaq);
    if (bmeOk)
        printValue(buffer, l.colR_value, l.rowBot2, color_iaq, bme_data.staticIaq);
    else
    {
        display.setTextColor(GxEPD_BLACK);
        display.setCursor(l.colR_value, l.rowBot2); display.print("--");
    }

    // Footer
    auto footerStr = [&](const String &s) -> String {
        return (l.footerMaxChars > 0 && s.length() > (unsigned)l.footerMaxChars)
            ? s.substring(0, l.footerMaxChars)
            : s;
    };

    display.setFont(&BabelSans8pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.drawInvertedBitmap(l.footerWlanIconX, l.footerWlanIconY, bitmap_wlan, 18, 18, GxEPD_BLACK);
    display.setCursor(l.footerWlanX, l.footerWlanY);
    display.print(footerStr(wlan_ssid));
    display.setCursor(l.footerIpX, l.footerIpY);
    display.print(footerStr("IP: " + ip_address));
    display.setCursor(l.footerDateX, l.footerDateY);
    display.print(footerStr(epd_date + " " + epd_time));
    display.drawInvertedBitmap(l.footerTurtleX, l.footerTurtleY, bitmap_turtle, 18, 18, GxEPD_RED);
    display.setCursor(l.footerNameX, l.footerNameY);
    display.print(footerStr(String(DeviceName)));
    display.display(false);
    display.hibernate();
    display.end();
}

void EPDHandler::updateEPD(const DataCO2 &co2, const Bsec &bme_data, const String &epd_date, const String &epd_time, const String &wlan_ssid, const String &ip_address, unsigned long currentSeconds)
{
    if (currentSeconds - _lastRunSeconds < (unsigned long)configHandler.getConfigInterval("intervalEPD"))
        return;

    _lastRunSeconds = currentSeconds;
    bool bmeOk  = BME680Handler::getInstance().isSensorOk();
    bool wifiOk = (WiFi.status() == WL_CONNECTED);
    String ssid    = wifiOk ? wlan_ssid            : "no WLAN";
    String ip      = wifiOk ? ip_address           : "---";
    String date    = wifiOk ? epd_date             : "---";
    String time    = wifiOk ? epd_time             : "--:--";

    int orientation = configHandler.getConfigSwitch("switchEPDorientation");
    const EPDLayout layout =
        (orientation == 1) ? layout_vertical(0)   :
        (orientation == 2) ? layout_horizontal(1) :
        (orientation == 3) ? layout_horizontal(3) :
                             layout_vertical(2);

    #if EPD_PIN_BUSY >= 0
        // BUSY pin wired: display.display() uses real busy signal, short wait
        if (_taskRunning) return;
        _taskRunning = true;
        printLayout(co2, bme_data, date, time, ssid, ip, bmeOk, layout);
        _taskRunning = false;
    #else
        // BUSY pin not wired: blocking delay ~2-3s, must run in FreeRTOS task
        if (_taskRunning) return;
        _taskRunning = true;
        EPDTaskParams *params = new EPDTaskParams{this, co2, bme_data, date, time, ssid, ip, bmeOk, layout};
        xTaskCreate(EPDHandler::epdRenderTask, "epd_render", 4096, params, 1, nullptr);
    #endif
    }

void EPDHandler::wipeDisplay()
{
    if (_wiped || _taskRunning) return;
    _wiped = true;
    _taskRunning = true;

    int ori = configHandler.getConfigSwitch("switchEPDorientation");
    EPDLayout l =
        (ori == 1) ? layout_vertical(0)   :
        (ori == 2) ? layout_horizontal(1) :
        (ori == 3) ? layout_horizontal(3) :
                     layout_vertical(2);

    #if EPD_PIN_BUSY >= 0
        display.init(BAUDRATE);
        display.fillScreen(GxEPD_WHITE);
        display.setFullWindow();
        display.setRotation(l.rotation);
        display.drawInvertedBitmap(l.footerTurtleX,   l.footerTurtleY,   bitmap_turtlesleep, 18, 18, GxEPD_RED);
        display.drawInvertedBitmap(l.footerWlanIconX, l.footerWlanIconY, bitmap_wlan,        18, 18, GxEPD_BLACK);
        display.display(false);
        display.hibernate();
        display.end();
        _taskRunning = false;
    #else
        EPDWipeParams *wp = new EPDWipeParams{this, l};
        xTaskCreate(epdWipeTask, "epd_wipe", 4096, wp, 1, nullptr);
    #endif
}

void EPDHandler::epdWipeTask(void *pvParameters)
{
    EPDWipeParams *p = static_cast<EPDWipeParams *>(pvParameters);
    display.init(BAUDRATE);
    display.fillScreen(GxEPD_WHITE);
    display.setFullWindow();
    display.setRotation(p->l.rotation);
    display.drawInvertedBitmap(p->l.footerTurtleX,   p->l.footerTurtleY,   bitmap_turtlesleep, 18, 18, GxEPD_RED);
    display.display(false);
    display.hibernate();
    display.end();
    p->h->_taskRunning = false;
    delete p;
    vTaskDelete(nullptr);
}

void EPDHandler::forceRefresh()
{
    _lastRunSeconds = 0;
    _wiped = false;
    _pendingRefresh = true;
    _taskRunning = false;
}