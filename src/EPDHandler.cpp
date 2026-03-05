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

// Returns GxEPD_RED if value exceeds threshold, otherwise GxEPD_BLACK
uint16_t EPDHandler::getAlertColor(float value, float threshold)
{
    return (value >= threshold) ? GxEPD_RED : GxEPD_BLACK;
}

uint16_t EPDHandler::getAlertColorInt(int value, int threshold)
{
    return (value >= threshold) ? GxEPD_RED : GxEPD_BLACK;
}

// Prints a single float value at the given position with color
void EPDHandler::printValue(char *buff, int16_t x, int16_t y, uint16_t color, float value)
{
    display.setTextColor(color);
    display.setCursor(x, y);
    snprintf(buff, EPD_FLOAT_BUFFER_SIZE, "%.1f", value);
    display.print(buff);
}

void EPDHandler::printVertically(const DataCO2 &co2, const Bsec &bme_data, const String &epd_date, const String &epd_time, bool bmeOk)
{
    display.init(BAUDRATE);
    display.fillScreen(GxEPD_WHITE);
    display.setFullWindow();
    display.setRotation(2);

    const int16_t col0 = 0;
    const int16_t col1 = 32;
    const int16_t col2 = 104;
    const int16_t rowTemp = 35;
    const int16_t rowHum  = 35 + 45;
    const int16_t rowIaq  = 35 + 90;
    const int16_t rowCo2  = 35 + 135;

    uint16_t color_temp = getAlertColor(bme_data.temperature, 26);
    uint16_t color_hum  = getAlertColor(bme_data.humidity, 70);
    uint16_t color_aiq  = getAlertColorInt((int)bme_data.iaq, 300);
    uint16_t color_co2  = getAlertColorInt(co2.getRegular(), 1500);

    char buffer[EPD_FLOAT_BUFFER_SIZE];
    display.setFont(&Inter_Bold12pt7b);

    if (bmeOk)
    {
        printValue(buffer, col1, rowTemp, color_temp, bme_data.temperature);
        display.drawInvertedBitmap(col2 - 1, rowTemp - 10, bitmap_grad18, 18, 18, color_temp);
        printValue(buffer, col1, rowHum, color_hum, bme_data.humidity);
        printValue(buffer, col1, rowIaq, color_aiq, bme_data.iaq);
    }
    else
    {
        display.setTextColor(GxEPD_BLACK);
        display.setCursor(col1, rowTemp); display.print("--");
        display.setCursor(col1, rowHum);  display.print("--");
        display.setCursor(col1, rowIaq);  display.print("--");
    }

    display.setFont(&Inter_Bold12pt7b);
    display.setTextColor(color_co2);
    display.setCursor(col1, rowCo2 + 5);
    snprintf(buffer, sizeof(buffer), "%d", co2.getRegular());
    display.print(buffer);

    display.setFont(&Inter_Regular8pt7b);
    display.setTextColor(color_hum);
    display.setCursor(col2, rowHum);
    display.print("%");

    display.drawInvertedBitmap(col2,  rowIaq - 10,  bitmap_iaq,   18, 18, color_aiq);
    display.drawInvertedBitmap(col2,  rowCo2 - 10,  bitmap_ppm18, 18, 18, color_co2);
    display.drawInvertedBitmap(col0,  rowTemp - 20, bitmap_temp,  24, 24, color_temp);
    display.drawInvertedBitmap(col0,  rowHum - 20,  bitmap_hum,   24, 24, color_hum);
    display.drawInvertedBitmap(col0,  rowIaq - 20,  bitmap_aiq,   24, 24, color_aiq);
    display.drawInvertedBitmap(col0,  rowCo2 - 14,  bitmap_CO2,   24, 24, color_co2);

    display.setRotation(2);
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&BabelSans8pt7b);
    display.setCursor(col0, 228);
    display.print(epd_date);

    display.setFont(&BabelSans10pt7b);
    display.setCursor(col0 + 80, 228);
    display.print(epd_time);

    display.setFont(&BabelSans8pt7b);
    display.setCursor(col0 + 20, 248);
    display.print(DeviceName);
    display.drawInvertedBitmap(col0, 248 - 14, bitmap_turtle, 18, 18, GxEPD_RED);

    display.display(false);
    display.hibernate();
    display.end();
}

void EPDHandler::printHorizontally(const DataCO2 &co2, const Bsec &bme_data, const String &epd_date, const String &epd_time, bool bmeOk)
{
    display.init(BAUDRATE);
    display.fillScreen(GxEPD_WHITE);
    display.setFullWindow();
    display.setRotation(1);

    const int col1 = 30;
    const int col2 = 90;
    const int rowTemp = 20;
    const int rowHum  = 52;
    const int rowIaq  = 86;
    const int rowCo2  = 115;

    uint16_t color_temp = getAlertColor(bme_data.temperature, 26);
    uint16_t color_hum  = getAlertColor(bme_data.humidity, 70);
    uint16_t color_aiq  = getAlertColorInt((int)bme_data.iaq, 300);
    uint16_t color_co2  = getAlertColorInt(co2.getRegular(), 1500);

    char buffer[EPD_FLOAT_BUFFER_SIZE];
    display.setFont(&Inter_Bold12pt7b);

    if (bmeOk)
    {
        printValue(buffer, col1, rowTemp, color_temp, bme_data.temperature);
        printValue(buffer, col1, rowHum,  color_hum,  bme_data.humidity);
        printValue(buffer, col1, rowIaq,  color_aiq,  bme_data.staticIaq);
    }
    else
    {
        display.setTextColor(GxEPD_BLACK);
        display.setCursor(col1, rowTemp); display.print("--");
        display.setCursor(col1, rowHum);  display.print("--");
        display.setCursor(col1, rowIaq);  display.print("--");
    }

    display.setFont(&Inter_Bold12pt7b);
    display.setTextColor(color_co2);
    display.setCursor(col1, rowCo2 + 5);
    snprintf(buffer, sizeof(buffer), "%d", co2.getRegular());
    display.print(buffer);

    display.setFont(&Inter_Regular10pt7b);
    display.setTextColor(color_hum);
    display.setCursor(col2, rowHum);
    display.print(" %");

    display.drawInvertedBitmap(col2 + 5, rowIaq  - 10, bitmap_iaq,   18, 18, color_aiq);
    display.drawInvertedBitmap(col2 + 5, rowCo2  - 10, bitmap_ppm18, 18, 18, color_co2);
    display.drawInvertedBitmap(col2 + 5, rowTemp - 20, bitmap_grad,  24, 24, color_temp);
    display.drawInvertedBitmap(0, rowTemp - 20, bitmap_temp, 24, 24, color_temp);
    display.drawInvertedBitmap(0, rowHum  - 20, bitmap_hum,  24, 24, color_hum);
    display.drawInvertedBitmap(0, rowIaq  - 20, bitmap_aiq,  24, 24, color_aiq);
    display.drawInvertedBitmap(0, rowCo2  - 14, bitmap_CO2,  24, 24, color_co2);

    display.setRotation(1);
    display.setFont(&BabelSans9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(140, 14);
    display.print(epd_time);
    display.setCursor(200, 14);
    display.print(epd_date);
    display.setCursor(140 + 20, 32);
    display.setFont(&BabelSans8pt7b);
    display.print(DeviceName);
    display.drawInvertedBitmap(140, 32 - 14, bitmap_turtle, 18, 18, GxEPD_RED);

    display.display(false);
    display.hibernate();
    display.end();
}

void EPDHandler::updateEPD(const DataCO2 &co2, const Bsec &bme_data, const String &epd_date, const String &epd_time, unsigned long currentSeconds)
{
    if (currentSeconds - _lastRunSeconds < (unsigned long)configHandler.getConfigInterval("intervalEPD"))
        return;

    _lastRunSeconds = currentSeconds;
    bool bmeOk = BME680Handler::getInstance().isSensorOk();
    bool horizontal = configHandler.getConfigSwitch("switchEPDorientation");

    if (horizontal)
        printHorizontally(co2, bme_data, epd_date, epd_time, bmeOk);
    else
        printVertically(co2, bme_data, epd_date, epd_time, bmeOk);
}

void EPDHandler::forceRefresh()
{
    // Set to a value that guarantees immediate update on next loop
    _lastRunSeconds = ULONG_MAX - (unsigned long)configHandler.getConfigInterval("intervalEPD") - 1;
}

void EPDHandler::clearDisplay()
{
    display.init(BAUDRATE);
    display.fillScreen(GxEPD_WHITE);
    display.setFullWindow();
    display.setRotation(2);
    display.drawInvertedBitmap(0, 248 - 14, bitmap_turtle, 18, 18, GxEPD_RED);
    display.display(false);
    display.hibernate();
    display.end();
}