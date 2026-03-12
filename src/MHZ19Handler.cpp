#include "MHZ19Handler.h" //myMHZ19.calibrate();    // Take a reading which be used as the zero point f
#include "ConfigHandler.h"
extern ConfigHandler &configHandler;

const String name_MHZ19_co2               = "[MHZ19] CO2 [ppm]";
const String name_MHZ19_co2_raw           = "[MHZ19] CO2 raw [ppm]";
const String name_MHZ19_co2_limited       = "[MHZ19] limited";
const String name_MHZ19_co2_background    = "[MHZ19] Background";
const String name_MHZ19_co2_tempAdjust    = "[MHZ19] Temperature Adjust";
const String name_MHZ19_co2_temperatur    = "[MHZ19] Temperature [C]";
const String name_MHZ19_co2_Accuracy      = "[MHZ19] Accuracy";
const String name_MHZ19_timestamp         = "[MHZ19] Timestamp [ms]";
const String name_MHZ19_datetime          = "[MHZ19] Date and Time";
const String name_MHZ19_date              = "[MHZ19] Date";
const String name_MHZ19_time              = "[MHZ19] Time";
const String name_MHZ19_zone              = "[MHZ19] Timezone";

MHZ19Handler::MHZ19Handler()
{
	Serial_MHZ19 = &Serial2;
	Serial2.begin(BAUDRATE, SERIAL_8N1, PIN_MHZ19_RX, PIN_MHZ19_TX); // Uno Example: Begin Stream with MHZ19 baudrate
	myMHZ19 = MHZ19();
	myMHZ19.begin(*Serial_MHZ19); // *Important, Pass your Stream reference
	// myMHZ19.printCommunication();                            // Error Codes are also included here if found (mainly for debugging/interest)

	myMHZ19.autoCalibration(true);

	Serial.print("[MHZ19] ABC Status: ");
	myMHZ19.getABC() ? Serial.println("ON") : Serial.println("OFF"); // now print it's status

	char myVersion[4];
	myMHZ19.getVersion(myVersion);

	Serial.print("[MHZ19] Range: ");
	Serial.println(myMHZ19.getRange());

	myMHZ19.verify();
	_lastReadout = DataCO2();
}

void MHZ19Handler::calibrate()
{
	// CALIBRATION
	// reset the MH-Z19B sensor by connecting "GND" pin and the "HD" pin for 7-10 seconds!!! 
	// This worked and I calibrated the sensor by running him at the open window and it is now starting up with 400~410 PPM.
	Serial.println("[MHZ19] Calibrating...");
	myMHZ19.calibrate();    // Take a reading which be used as the zero point for 400 ppm^
}




void MHZ19Handler::printoutCurrentValues()
{
	if (myMHZ19.errorCode == RESULT_OK)
	{
		Serial.println("[MHZ19] CurrentValues:");
		//        Serial.println(name_MHZ19_timestamp       + ":     "       + data_MHZ19_timestamp     );
		//        Serial.println(name_MHZ19_datetime        + ":      "      + data_MHZ19_datetime      );
        Serial.println(name_MHZ19_co2             + ":          "   + myMHZ19.getCO2()            );
        Serial.println(name_MHZ19_co2_raw         + ":       "      + myMHZ19.getCO2Raw()         );
        Serial.println(name_MHZ19_co2_limited     + ":            " + myMHZ19.getCO2(false)       );
        Serial.println(name_MHZ19_co2_background  + ":         "    + myMHZ19.getBackgroundCO2()  );
        Serial.println(name_MHZ19_co2_tempAdjust  + ": "            + myMHZ19.getTempAdjustment() );
        Serial.println(name_MHZ19_co2_temperatur  + ":    "         + myMHZ19.getTemperature()    );
        Serial.println(name_MHZ19_co2_Accuracy    + ":           "  + myMHZ19.getAccuracy()       );
		Serial.println();
	}
	else
	{
		Serial.printf("[MHZ19] Read failed – errorCode=%d (%d consecutive)\n", myMHZ19.errorCode, _consecutiveErrors);
	}
}

void MHZ19Handler::printoutLastReadout()
{
	Serial.println("[MHZ19] LastReadout:");
        Serial.println(name_MHZ19_co2             + ":          "   + String(_lastReadout.getRegular()       ));
        Serial.println(name_MHZ19_co2_raw         + ":      "       + String(_lastReadout.getRaw()           ));
        Serial.println(name_MHZ19_co2_limited     + ":            " + String(_lastReadout.getLimited()       ));
        Serial.println(name_MHZ19_co2_background  + ":         "    + String(_lastReadout.getBackground()    ));
        Serial.println(name_MHZ19_co2_tempAdjust  + ": "            + String(_lastReadout.getTempAdjustment()));
        Serial.println(name_MHZ19_co2_temperatur  + ":    "         + String(_lastReadout.getTemperature()   ));
        Serial.println(name_MHZ19_co2_Accuracy    + ":           "  + String(_lastReadout.getAccuracy()      ));
	Serial.println();
}

DataCO2 MHZ19Handler::getLastReadout()
{
	return _lastReadout;
}

bool MHZ19Handler::updateLastReadout()
{
	if (myMHZ19.errorCode == RESULT_OK)
	{
		_consecutiveErrors = 0;
		_lastReadout = DataCO2(
			myMHZ19.getCO2(),
			myMHZ19.getCO2Raw(),
			myMHZ19.getCO2(false),
			myMHZ19.getBackgroundCO2(),
			myMHZ19.getTempAdjustment(),
			myMHZ19.getTemperature(),
			myMHZ19.getAccuracy());
		return true;
	}
	else
	{
		_consecutiveErrors++;
		Serial.printf("[MHZ19] Error count %d, response code %d\n", _consecutiveErrors, myMHZ19.errorCode);

		if (_consecutiveErrors >= 3)
		{
			Serial.println("[MHZ19] Recovery: reinitializing Serial2...");
			Serial_MHZ19->end();
			vTaskDelay(500 / portTICK_PERIOD_MS);
			Serial2.begin(BAUDRATE, SERIAL_8N1, PIN_MHZ19_RX, PIN_MHZ19_TX);
			myMHZ19.begin(*Serial_MHZ19);
			_consecutiveErrors = 0;
		}
		return false;
	}
}

bool MHZ19Handler::runUpdate(const unsigned long currentSeconds)
{
	if (currentSeconds - _lastRunSeconds >=
		(unsigned long)configHandler.getConfigInterval("intervalMHZ19"))
	{
		_lastRunSeconds = currentSeconds;
		return updateLastReadout();
	}
	return false;
}
