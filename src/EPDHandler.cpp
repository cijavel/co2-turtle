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

extern ConfigHandler &configHandler;

#define EPD_FLOAT_BUFFER_SIZE 8  // enough for "-123.4\0"

uint16_t EPDHandler::getAlertColor(float value, float threshold)
{
    return (value >= threshold) ? GxEPD_RED : GxEPD_BLACK;
}

uint16_t EPDHandler::getAlertColorInt(int value, int threshold)
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

EPDLayout EPDHandler::layout_V90()
{
    EPDLayout l;
    l.rotation      = 2;
    l.colL_icon     = 2;    l.colL_value = 30;   l.colL_unit = 101;
    l.colR_icon     = 2;    l.colR_value = 30;   l.colR_unit = 101;
    l.rowTop        = 30;
    l.rowBot        = 68;
    l.rowTop2       = 106;
    l.rowBot2       = 144;
    // Footer zone Y 160–250, 5 elements, spacing 15px
    l.footerWlanIconX = 1;  l.footerWlanIconY = 176;
    l.footerWlanX   = 20;    l.footerWlanY   = 191;
    l.footerIpX     = 1;    l.footerIpY     = 209;
    l.footerDateX   = 1;    l.footerDateY   = 227;
    l.footerTurtleX = 1;    l.footerTurtleY = 230;
    l.footerNameX   = 20;   l.footerNameY   = 245;
    l.footerMaxChars = 17;
    return l;
}

EPDLayout EPDHandler::layout_V270()
{
    EPDLayout l;
    l.rotation      = 0;
    // Mirror of V90: X = 122 - x_90 - elemW, Y = 250 - y_90 - elemH

    // Sensor columns (icon 24px wide, value ~30px, unit 18px)
    l.colL_icon     = 122 - 2   - 24;  // 96
    l.colL_value    = 122 - 30  - 30;  // 62
    l.colL_unit     = 122 - 101 - 18;  // 3
    l.colR_icon     = l.colL_icon;
    l.colR_value    = l.colL_value;
    l.colR_unit     = l.colL_unit;

    // Sensor rows (font baseline, mirrored: 250 - y_90 - fontH(~20))
    l.rowTop        = 250 - 30  - 20;  // 200  Temp
    l.rowBot        = 250 - 68  - 20;  // 162  Hum
    l.rowTop2       = 250 - 106 - 20;  // 124  CO2
    l.rowBot2       = 250 - 144 - 20;  // 86   IAQ

    // Footer zone – visually at bottom = small Y values (250 - y_90 - elemH)
    // Order reversed: Name at top (small Y), WLAN at bottom (larger Y)
    l.footerTurtleX = 122 - 1   - 18;  // 103
    l.footerTurtleY = 250 - 245 - 18;  // 5    (mirrors nameY=245)
    l.footerNameX   = 122 - 20  - 60;  // 42   (approx name text width 60)
    l.footerNameY   = 250 - 245 - 12;  // 5
    l.footerDateX   = 122 - 1   - 70;  // 51   (date+time ~70px wide)
    l.footerDateY   = 250 - 227 - 12;  // 11
    l.footerIpX     = 122 - 1   - 80;  // 41
    l.footerIpY     = 250 - 209 - 12;  // 29
    l.footerWlanIconX = 122 - 1  - 18; // 103
    l.footerWlanIconY = 250 - 194 - 18;// 38   (mirrors wlanIconY=176+18)
    l.footerWlanX   = 122 - 20  - 70;  // 32
    l.footerWlanY   = 250 - 191 - 12;  // 47
    l.footerMaxChars = 17;
    return l;
}

EPDLayout EPDHandler::layout_H90()
{
    EPDLayout l;
    l.rotation      = 1;
    // Left column: Temp + Hum
    l.colL_icon     = 0;    l.colL_value = 30;   l.colL_unit = 90;
    // Right column: CO2 + IAQ
    l.colR_icon     = 130;  l.colR_value = 160;  l.colR_unit = 220;
    // 2 shared rows for both columns
    l.rowTop        = 38;   l.rowTop2 = 38;
    l.rowBot        = 75;   l.rowBot2 = 75;
    // Footer zone Y 85–122
    l.footerWlanX   = 0;    l.footerWlanY   = 95;
    l.footerIpX     = 0;    l.footerIpY     = 107;
    l.footerDateX   = 0;    l.footerDateY   = 119;
    l.footerTurtleX = 232;  l.footerTurtleY = 103;
    l.footerNameX   = 20;   l.footerNameY   = 119;
    return l;
}

EPDLayout EPDHandler::layout_H270()
{
    EPDLayout l;
    l.rotation      = 3;
    // Mirrored: X = 250 - orig_x - width, Y = 122 - orig_y
    // Left column mirrored (was right in H90)
    l.colL_icon     = 96;   l.colL_value = 65;   l.colL_unit = 12;
    // Right column mirrored (was left in H90)
    l.colR_icon     = 226;  l.colR_value = 195;  l.colR_unit = 142;
    // Rows mirrored
    l.rowTop        = 84;   l.rowTop2 = 84;   // 122-38
    l.rowBot        = 47;   l.rowBot2 = 47;   // 122-75
    // Footer zone – visually at bottom = small Y values
    l.footerWlanX   = 0;    l.footerWlanY   = 8;
    l.footerIpX     = 0;    l.footerIpY     = 20;
    l.footerDateX   = 0;    l.footerDateY   = 32;
    l.footerTurtleX = 232;  l.footerTurtleY = 103;
    l.footerNameX   = 20;   l.footerNameY   = 118;
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

    uint16_t color_temp = getAlertColor(bme_data.temperature, 26);
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
    bool bmeOk = BME680Handler::getInstance().isSensorOk();

    int orientation = configHandler.getConfigSwitch("switchEPDorientation");
    const EPDLayout layout =
        (orientation == 1) ? layout_V270() :
        (orientation == 2) ? layout_H90()  :
        (orientation == 3) ? layout_H270() :
                             layout_V90();

    if (_taskRunning) return;  // previous render still in progress
    _taskRunning = true;

    EPDTaskParams *params = new EPDTaskParams{
        this, co2, bme_data, epd_date, epd_time, wlan_ssid, ip_address, bmeOk, layout
    };
    xTaskCreate(EPDHandler::epdRenderTask, "epd_render", 4096, params, 1, nullptr);
}

void EPDHandler::wipeDisplay()
{
    if (_wiped || _taskRunning) return;
    _wiped = true;
    _taskRunning = true;

    int ori = configHandler.getConfigSwitch("switchEPDorientation");
    EPDWipeParams *wp = new EPDWipeParams{this,
        (ori == 1) ? layout_V270() :
        (ori == 2) ? layout_H90()  :
        (ori == 3) ? layout_H270() :
                     layout_V90()};
    xTaskCreate(epdWipeTask, "epd_wipe", 4096, wp, 1, nullptr);
}

void EPDHandler::epdWipeTask(void *pvParameters)
{
    EPDWipeParams *p = static_cast<EPDWipeParams *>(pvParameters);
    display.init(BAUDRATE);
    display.fillScreen(GxEPD_WHITE);
    display.setFullWindow();
    display.setRotation(p->l.rotation);
    display.drawInvertedBitmap(p->l.footerTurtleX, p->l.footerTurtleY, bitmap_turtlesleep, 18, 18, GxEPD_RED);
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
}