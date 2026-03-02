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
	static void updateEPDvertical(DataCO2 co2, Bsec data, const String &epd_date, const String &epd_time, unsigned long currentSeconds);
	static void updateEPDhorizontal(DataCO2 co2, Bsec data, const String &epd_date, const String &epd_time, unsigned long currentSeconds);

private:
	static void printVertically(DataCO2 co2, Bsec bme_data, const String &epd_date, const String &epd_time, bool bmeOk);
	static void printHorizontally(DataCO2 co2, Bsec bme_data, const String &epd_date, const String &epd_time, bool bmeOk);
	EPDHandler() {};
	EPDHandler(EPDHandler const &);
	void operator=(EPDHandler const &);
	static void PrintEspLine(char *buff, int16_t cursorX, int16_t cursorY, uint16_t color, float toPrint);
	static unsigned long _lastRunSeconds;
};
#endif // CO2_TURTLE_EPDHANDLER_H
