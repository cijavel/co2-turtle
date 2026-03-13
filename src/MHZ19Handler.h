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



private:
	bool updateLastReadout();
	DataCO2 _lastReadout;
	MHZ19 myMHZ19;
	HardwareSerial *Serial_MHZ19;
	int _consecutiveErrors = 0;
	unsigned long _lastRunSeconds = 0;
	MHZ19Handler();						  // Constructor? (the {} brackets) are needed here.
	MHZ19Handler(MHZ19Handler const &);	  // Don't Implement
	void operator=(MHZ19Handler const &); // Don't implement

};
#endif // CO2_TURTLE_MHZ19HANDLER_H
