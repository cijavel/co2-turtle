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
#include "symbol.h"
#include "Configuration.h"

void EPDHandler::printVertically(const CO2Data co2, const Bsec data, const String& epd_date, const String& epd_time) {
    display.init(9600);

    uint16_t color_temp = GxEPD_BLACK;
    uint16_t color_hum = GxEPD_BLACK;
    uint16_t color_aiq = GxEPD_BLACK;
    uint16_t color_co2 = GxEPD_BLACK;

    // Lines
    int line0 = 0;
    int line1 = 32;
    int line2 = 104;

    //Abstand
    int ab_temp = 35;
    int ab_hum  = ab_temp + 45;
    int ab_iaq  = ab_hum  + 45;
    int ab_co2  = ab_iaq  + 45;

    //Limit
    if (data.temperature < 26) {
        color_temp = GxEPD_BLACK;
    }
    else {
        color_temp = GxEPD_RED;
    }
    if (data.humidity < 70){
        color_hum  = GxEPD_BLACK;
    }
    else {
        color_hum  = GxEPD_RED;
    }
    if (data.iaq < 300) {
        color_aiq  = GxEPD_BLACK;
    }
    else {
        color_aiq  = GxEPD_RED;
    }
    if (co2.getRegular() < 1500) {
        color_co2  = GxEPD_BLACK;
    }
    else {
        color_co2  = GxEPD_RED;
    }


    display.fillScreen(GxEPD_WHITE); // set the background to white (fill the buffer with value for white)
    display.setFullWindow();
    display.setCursor(0, 0);

    display.setRotation(2); //0-3
    display.setFont(&Inter_Bold12pt7b);

    display.setTextColor(color_temp);
    display.setCursor(line1, ab_temp);
    char buffer[5];
    snprintf(buffer , sizeof buffer, "%f", data.temperature);
    display.print(buffer);
    display.drawInvertedBitmap(line2-1, ab_temp-10, bitmap_grad18, 18, 18, color_temp);


    display.setFont(&Inter_Bold12pt7b);
    display.setTextColor(color_hum);
    display.setCursor(line1, ab_hum );
    snprintf(buffer , sizeof buffer, "%f", data.humidity);
    display.print(buffer);


    display.setFont(&Inter_Bold12pt7b);
    display.setTextColor(color_aiq);
    display.setCursor(line1, ab_iaq );
    snprintf(buffer , sizeof buffer, "%f", data.iaq);
    display.print(buffer);


    display.setFont(&Inter_Bold12pt7b);
    display.setTextColor(color_co2);
    display.setCursor(line1, ab_co2 + 5);
    snprintf(buffer , sizeof buffer, "%d", co2.getRegular());
    display.print(buffer);



    display.setFont(&Inter_Regular8pt7b);
    display.setTextColor(color_hum);
    display.setCursor(line2, ab_hum);
    display.print("%");

    display.setTextColor(color_aiq);
    display.setCursor(line2, ab_iaq);
    display.drawInvertedBitmap(line2  , ab_iaq-10, bitmap_iaq , 18, 18, color_aiq);

    display.drawInvertedBitmap(line2, ab_co2-10, bitmap_ppm18, 18, 18, color_co2);

    display.drawInvertedBitmap(line0  , ab_temp-20, bitmap_temp, 24, 24, color_temp);
    display.drawInvertedBitmap(line0  , ab_hum -20, bitmap_hum , 24, 24, color_hum);
    display.drawInvertedBitmap(line0  , ab_iaq -20, bitmap_aiq , 24, 24, color_aiq);
    display.drawInvertedBitmap(line0  , ab_co2 -14, bitmap_CO2 , 24, 24, color_co2);


    display.setRotation(2); //0-3
    display.setTextColor(GxEPD_BLACK);

    display.setFont(&BabelSans8pt7b);
    display.setCursor(line0, 228);
    display.print(epd_date);

    display.setFont(&BabelSans10pt7b);
    display.setCursor(line0+80, 228);
    display.print(epd_time);

    display.setFont(&BabelSans8pt7b);
    display.setCursor(line0+20, 248);
    display.print(DeviceName);
    display.drawInvertedBitmap(line0  , 248-14, bitmap_turtle, 18, 18, GxEPD_RED);

    display.hibernate();
    display.display(false);
}
