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

EPDLayout EPDHandler::verticalLayout(int rotation)
{
    EPDLayout l;
    l.rotation = rotation;

    if (rotation == 2) // vertical 90° – origin top-left
    {
        l.colL_icon   = 0;
        l.colL_value  = 32;
        l.colL_unit   = 104;
        l.colR_icon   = 0;
        l.colR_value  = 32;
        l.colR_unit   = 104;
        l.rowTop      = 35;
        l.rowBot      = 80;
        l.rowTop2     = 125;
        l.rowBot2     = 170;
        l.footerTurtleX = 0;  l.footerTurtleY = 242;
        l.footerWlanX   = 0;  l.footerWlanY   = 206;
        l.footerIpX     = 0;  l.footerIpY     = 218;
        l.footerDateX   = 0;  l.footerDateY   = 230;
        l.footerNameX   = 20; l.footerNameY   = 248;
    }
    else // rotation == 0, vertical 270° – origin bottom-right, mirror all coords
    // Display is 122wide x 250high. Mirrored: x = 122-x-w, y = 250-y-h
    {
        l.colL_icon   = 122 - 0   - 24; 
        l.colL_value  = 122 - 32  - 30; 
        l.colL_unit   = 122 - 104 - 18; 
        l.colR_icon   = 122 - 0   - 24; 
        l.colR_value  = 122 - 32  - 30; 
        l.colR_unit   = 122 - 104 - 18; 
        l.rowTop      = 250 - 35  - 20; 
        l.rowBot      = 250 - 80  - 20; 
        l.rowTop2     = 250 - 125 - 20; 
        l.rowBot2     = 250 - 170 - 20;         
        l.footerTurtleX = 104; l.footerTurtleY = 2;
        l.footerWlanX   = 0;   l.footerWlanY   = 26;
        l.footerIpX     = 0;   l.footerIpY     = 14;
        l.footerDateX   = 0;   l.footerDateY   = 2;
        l.footerNameX   = 20;  l.footerNameY   = 38;
    }
    return l;
}

EPDLayout EPDHandler::horizontalLayout(int rotation)
{
    EPDLayout l;
    l.rotation = rotation;

    if (rotation == 1) // horizontal 90° – origin top-left
    {
        l.colL_icon   = 0;
        l.colL_value  = 30;
        l.colL_unit   = 90;
        l.colR_icon   = 130;
        l.colR_value  = 160;
        l.colR_unit   = 225;
        l.rowTop      = 20;
        l.rowBot      = 52;
        l.rowTop2     = l.rowTop;
        l.rowBot2     = l.rowBot;
        l.footerTurtleX = 2;   l.footerTurtleY = 118;
        l.footerWlanX   = 2;   l.footerWlanY   = 82;
        l.footerIpX     = 2;   l.footerIpY     = 94;
        l.footerDateX   = 2;   l.footerDateY   = 106;
        l.footerNameX   = 22;  l.footerNameY   = 118;
    }
    else // rotation == 3, horizontal 270° – origin bottom-right
    // Display is 250wide x 122high in landscape. Mirrored: x = 250-x-w, y = 122-y-h
    {
        l.colL_icon   = 250 - 0   - 24;
        l.colL_value  = 250 - 30  - 30;
        l.colL_unit   = 250 - 90  - 18;
        l.colR_icon   = 250 - 130 - 24;
        l.colR_value  = 250 - 160 - 30;
        l.colR_unit   = 250 - 225 - 18;
        l.rowTop      = 122 - 20  - 20;
        l.rowBot      = 122 - 52  - 20;
        l.rowTop2     = l.rowTop;
        l.rowBot2     = l.rowBot;
        l.footerTurtleX = 230; l.footerTurtleY = 2;
        l.footerWlanX   = 2;   l.footerWlanY   = 30;
        l.footerIpX     = 2;   l.footerIpY     = 18;
        l.footerDateX   = 2;   l.footerDateY   = 6;
        l.footerNameX   = 22;  l.footerNameY   = 4;
    }
    return l;
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
    display.setFont(&BabelSans8pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(l.footerWlanX, l.footerWlanY);
    display.print("WLAN: " + wlan_ssid);
    display.setCursor(l.footerIpX, l.footerIpY);
    display.print("IP: " + ip_address);
    display.setCursor(l.footerDateX, l.footerDateY);
    display.print(epd_date + " " + epd_time);
    display.drawInvertedBitmap(l.footerTurtleX, l.footerTurtleY, bitmap_turtle, 18, 18, GxEPD_RED);
    display.setCursor(l.footerNameX, l.footerNameY);
    display.print(DeviceName);
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
    EPDLayout layout;
    switch (orientation)
    {
        case 1:  layout = verticalLayout(0);    break; // vertical 270°
        case 2:  layout = horizontalLayout(1);  break; // horizontal 90°
        case 3:  layout = horizontalLayout(3);  break; // horizontal 270°
        default: layout = verticalLayout(2);    break; // vertical 90°
    }

    printLayout(co2, bme_data, epd_date, epd_time, wlan_ssid, ip_address, bmeOk, layout);
}

void EPDHandler::wipeDisplay()
{
    if (_wiped) return;  // only wipe once
    const EPDLayout l =
        (configHandler.getConfigSwitch("switchEPDorientation") == 1) ? verticalLayout(0)   :
        (configHandler.getConfigSwitch("switchEPDorientation") == 2) ? horizontalLayout(1) :
        (configHandler.getConfigSwitch("switchEPDorientation") == 3) ? horizontalLayout(3) :
                                                                        verticalLayout(2);
    display.init(BAUDRATE);
    display.fillScreen(GxEPD_WHITE);
    display.setFullWindow();
    display.setRotation(l.rotation);
    display.drawInvertedBitmap(l.footerTurtleX, l.footerTurtleY, bitmap_turtle, 18, 18, GxEPD_RED);
    display.display(false);
    display.hibernate();
    display.end();
    _wiped = true;
}

void EPDHandler::forceRefresh()
{
    _lastRunSeconds = ULONG_MAX - (unsigned long)configHandler.getConfigInterval("intervalEPD") - 1;
    _wiped = false;  // allow update even if EPD was wiped
}