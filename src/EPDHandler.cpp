#include "EPDHandler.h"
#include <GxEPD2_3C.h>
#include "GxEPD2_display_selection_new_style.h"
#include "../Font/BabelSans8pt7b.h"
#include "../Font/BabelSans9pt7b.h"
#include "../Font/BabelSans10pt7b.h"
#include "../Font/Inter_Regular12pt7b.h"
#include "../Font/Inter_Regular11pt7b.h"
#include "../Font/Inter_Regular10pt7b.h"
#include "../Font/Inter_Regular8pt7b.h"
#include "../Font/Inter_Regular7pt7b.h"

#include "../Font/Inter_Bold12pt7b.h"
#include "../Font/Inter_Bold10pt7b.h"

#include "Configuration.h"
#include "symbol.h" // own symbol

#define EPDPrintFormatBufferSize 5

void EPDHandler::printVertically(const CO2Data co2, const Bsec bme_data, const String& epd_date, const String& epd_time) {
    display.init(BAUDRATE);

    uint16_t color_temp = GxEPD_BLACK;
    uint16_t color_hum = GxEPD_BLACK;
    uint16_t color_aiq = GxEPD_BLACK;
    uint16_t color_co2 = GxEPD_BLACK;

    // Lines
    int16_t line0 = 0;
    int16_t line1 = 32;
    int16_t line2 = 104;

    //Abstand
    int16_t ab_temp = 35;
    int16_t ab_hum = 35 + 45;
    int16_t ab_iaq = 35 + 45 + 45;
    int16_t ab_co2 = 35 + 45 + 45 + 45;

    //Limit
    if (bme_data.temperature < 26) {
        color_temp = GxEPD_BLACK;
    } else {
        color_temp = GxEPD_RED;
    }
    if (bme_data.humidity < 70) {
        color_hum = GxEPD_BLACK;
    } else {
        color_hum = GxEPD_RED;
    }
    if (bme_data.iaq < 300) {
        color_aiq = GxEPD_BLACK;
    } else {
        color_aiq = GxEPD_RED;
    }
    if (co2.getRegular() < 1500) {
        color_co2 = GxEPD_BLACK;
    } else {
        color_co2 = GxEPD_RED;
    }


    display.fillScreen(GxEPD_WHITE); // set the background to white (fill the buffer with value for white)
    display.setFullWindow();
    display.setCursor(0, 0);

    display.setRotation(2); //0-3
    char buffer[EPDPrintFormatBufferSize];
    display.setFont(&Inter_Bold12pt7b);
    PrintEspLine(buffer, line1, ab_temp, color_temp, bme_data.temperature);
    display.drawInvertedBitmap(line2 - 1, ab_temp - 10, bitmap_grad18, 18, 18, color_temp);

    PrintEspLine(buffer, line1, ab_hum, color_hum, bme_data.humidity);
    PrintEspLine(buffer, line1, ab_iaq, color_aiq, bme_data.iaq);

    display.setFont(&Inter_Bold12pt7b);
    display.setTextColor(color_co2);
    display.setCursor(line1, ab_co2 + 5);
    snprintf(buffer, sizeof buffer, "%d", co2.getRegular());
    display.print(buffer);

    display.setFont(&Inter_Regular8pt7b);
    display.setTextColor(color_hum);
    display.setCursor(line2, ab_hum);
    display.print("%");

    display.setTextColor(color_aiq);
    display.setCursor(line2, ab_iaq);
    display.drawInvertedBitmap(line2, ab_iaq - 10, bitmap_iaq, 18, 18, color_aiq);
    display.drawInvertedBitmap(line2, ab_co2 - 10, bitmap_ppm18, 18, 18, color_co2);
    display.drawInvertedBitmap(line0, ab_temp - 20, bitmap_temp, 24, 24, color_temp);
    display.drawInvertedBitmap(line0, ab_hum - 20, bitmap_hum, 24, 24, color_hum);
    display.drawInvertedBitmap(line0, ab_iaq - 20, bitmap_aiq, 24, 24, color_aiq);
    display.drawInvertedBitmap(line0, ab_co2 - 14, bitmap_CO2, 24, 24, color_co2);


    display.setRotation(2); //0-3
    display.setTextColor(GxEPD_BLACK);

    display.setFont(&BabelSans8pt7b);
    display.setCursor(line0, 228);
    display.print(epd_date);

    display.setFont(&BabelSans10pt7b);
    display.setCursor(line0 + 80, 228);
    display.print(epd_time);

    display.setFont(&BabelSans8pt7b);
    display.setCursor(line0 + 20, 248);
    display.print(DeviceName);
    display.drawInvertedBitmap(line0, 248 - 14, bitmap_turtle, 18, 18, GxEPD_RED);

    display.display(false);
    display.hibernate();
    display.end();
}

void EPDHandler::updateEPDvertical(const CO2Data co2, const Bsec bme_data, const String& epd_date, const String& epd_time, const unsigned long currentSeconds ) {
    if (currentSeconds % interval_EPD_in_Seconds == 0){
        printVertically(co2, bme_data, epd_date, epd_time);
    }

}

void EPDHandler::PrintEspLine(char* buff, int16_t cursorX, int16_t cursorY, uint16_t color, float toPrint) {
    display.setTextColor(color);
    display.setCursor(cursorX, cursorY);
    snprintf(buff, EPDPrintFormatBufferSize, "%f", toPrint);
    display.print(buff);
}
