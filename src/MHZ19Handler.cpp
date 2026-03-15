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

	// --- Variant detection ---
	int cfgVariant = configHandler.getConfigSensor("sensorMHZ19variant");
	if (cfgVariant == VARIANT_AUTO) {
		_variant = detectVariant();
	} else {
		_variant = static_cast<SensorVariant>(cfgVariant);
		Serial.printf("[MHZ19] Variant forced by config: %s\n", getSensorVariantName().c_str());
	}

	// autoCalibration (ABC) is only supported on MH-Z19B/C.
	// Calling it on the original may be silently ignored or cause undefined behaviour.
	if (_variant == VARIANT_BC) {
		myMHZ19.autoCalibration(true);
		Serial.print("[MHZ19] ABC Status: ");
		myMHZ19.getABC() ? Serial.println("ON") : Serial.println("OFF");
	} else {
		Serial.println("[MHZ19] Original variant: skipping autoCalibration (not supported).");
	}

	char myVersion[4];
	myMHZ19.getVersion(myVersion);

	Serial.print("[MHZ19] Range: ");
	Serial.println(myMHZ19.getRange());

	myMHZ19.verify();
	_lastReadout = DataCO2();
}

MHZ19Handler::SensorVariant MHZ19Handler::detectVariant()
{
	// The firmware-version command (0x3D) returns a non-zero 4-byte string on
	// MH-Z19B/C and all-zeros on the original MH-Z19.
	char version[4] = {0, 0, 0, 0};
	myMHZ19.getVersion(version);
	bool hasVersion = (version[0] != 0 || version[1] != 0 || version[2] != 0 || version[3] != 0);
	Serial.printf("[MHZ19] Version response: %c%c%c%c -> auto-detected: %s\n",
	              version[0] ? version[0] : '0', version[1] ? version[1] : '0',
	              version[2] ? version[2] : '0', version[3] ? version[3] : '0',
	              hasVersion ? "MH-Z19B/C" : "MH-Z19 (original)");
	return hasVersion ? VARIANT_BC : VARIANT_ORIGINAL;
}

String MHZ19Handler::getSensorVariantName() const
{
	switch (_variant) {
		case VARIANT_ORIGINAL: return "MH-Z19 (original)";
		case VARIANT_BC:       return "MH-Z19B/C";
		default:               return "unknown";
	}
}

void MHZ19Handler::calibrate()
{
	// CALIBRATION
	// reset the MH-Z19B sensor by connecting "GND" pin and the "HD" pin for 7-10 seconds!!!
	// This worked and I calibrated the sensor by running him at the open window and it is now starting up with 400~410 PPM.
	Serial.println("[MHZ19] Calibrating...");
	myMHZ19.calibrate();    // Take a reading which be used as the zero point for 400 ppm^
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
		// getCO2Raw(), getBackgroundCO2(), getTempAdjustment() are B/C-only.
		// The original MH-Z19 returns 0 or garbage for these commands.
		unsigned int co2Raw     = (_variant == VARIANT_BC) ? myMHZ19.getCO2Raw()         : 0;
		int          background = (_variant == VARIANT_BC) ? myMHZ19.getBackgroundCO2()  : 0;
		byte         tempAdjust = (_variant == VARIANT_BC) ? myMHZ19.getTempAdjustment() : 0;
		_lastReadout = DataCO2(
			myMHZ19.getCO2(),
			co2Raw,
			myMHZ19.getCO2(false),
			background,
			tempAdjust,
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
