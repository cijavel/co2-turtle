#ifndef CO2_TURTLE_MHZ19HANDLER_H
#define CO2_TURTLE_MHZ19HANDLER_H
#include <HardwareSerial.h>
#include "MHZ19.h"
#include "Arduino.h"
#include "Configuration.h"
#include "DataCO2.h"

class MHZ19Handler
{
public:
	static MHZ19Handler &getInstance()
	{
		static MHZ19Handler instance; // Guaranteed to be destroyed.
		return instance;			  // Instantiated on first use.
	}
	DataCO2 getLastReadout();
	bool runUpdate(unsigned long currentSeconds);
	void calibrate();
	void applyABC(bool enable);
	String getSensorVariantName() const;

	// 24h ABC session: enable ABC for exactly 24h, then revert to persistent switchABC.
	// Tracks lowest CO2 seen during the window so the user can tell whether the
	// sensor actually got close to fresh-air (~400 ppm).
	bool startABCSession();
	void cancelABCSession();
	bool isABCSessionActive() const { return _abcSessionActive; }
	bool isABCSessionAvailable() const { return _variant == VARIANT_BC; }
	unsigned long getABCSessionRemainingSeconds() const;
	int getABCSessionMinCO2() const { return _abcSessionMinCO2; }
	bool getABCSessionMinReached() const { return _abcSessionMinCO2 > 0 && _abcSessionMinCO2 <= 420; }
	static constexpr unsigned long ABC_SESSION_DURATION_S = 24UL * 60UL * 60UL;

	enum SensorVariant { VARIANT_AUTO = 0, VARIANT_ORIGINAL = 1, VARIANT_BC = 2 };

private:
	bool updateLastReadout();
	SensorVariant detectVariant();
	void finishABCSession();
	DataCO2 _lastReadout;
	MHZ19 myMHZ19;
	HardwareSerial *Serial_MHZ19;
	int _consecutiveErrors = 0;
	unsigned long _lastRunSeconds = 0;
	SensorVariant _variant = VARIANT_AUTO;
	bool _abcSessionActive = false;
	unsigned long _abcSessionStartMillis = 0;
	int _abcSessionMinCO2 = -1;
	MHZ19Handler();						  // Constructor? (the {} brackets) are needed here.
	MHZ19Handler(MHZ19Handler const &);	  // Don't Implement
	void operator=(MHZ19Handler const &); // Don't implement

};
#endif // CO2_TURTLE_MHZ19HANDLER_H
