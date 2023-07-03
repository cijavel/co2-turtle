#include "MHZ19Handler.h"

const String name_MHZ19_co2               = "MHZ19 CO2 [ppm]";
const String name_MHZ19_co2_raw           = "MHZ19 CO2 raw [ppm]";
const String name_MHZ19_co2_limited       = "MHZ19 limited";
const String name_MHZ19_co2_background    = "MHZ19 Background";
const String name_MHZ19_co2_tempAdjust    = "MHZ19 Temperature Adjust";
const String name_MHZ19_co2_temperatur    = "MHZ19 Temperature [C]";
const String name_MHZ19_co2_Accuracy      = "MHZ19 Accuracy";
const String name_MHZ19_timestamp         = "MHZ19 Timestamp [ms]";
const String name_MHZ19_datetime          = "MHZ19 Date and Time";
const String name_MHZ19_date              = "MHZ19 Date";
const String name_MHZ19_time              = "MHZ19 Time";
const String name_MHZ19_zone              = "MHZ19 Timezone";

MHZ19Handler::MHZ19Handler() {
    Serial_MHZ19 = new SoftwareSerial(PIN_MHZ19_RX, PIN_MHZ19_TX);
    Serial_MHZ19->begin(BAUDRATE);                                // Uno Example: Begin Stream with MHZ19 baudrate
    myMHZ19 = MHZ19();
    myMHZ19.begin(*Serial_MHZ19);                                 // *Important, Pass your Stream reference
    //myMHZ19.printCommunication();                            // Error Codes are also included here if found (mainly for debugging/interest)

    myMHZ19.autoCalibration(true);
#ifdef DEBUG
    Serial.print("MHZ19: ABC Status: ");
    myMHZ19.getABC() ? Serial.println("ON") :  Serial.println("OFF");  // now print it's status
#endif
    char myVersion[4];
    myMHZ19.getVersion(myVersion);
#ifdef DEBUG
    Serial.print("MHZ19: Range: ");
    Serial.println(myMHZ19.getRange());
#endif
    //Serial.println(""MHZ19: Calibrating..");
    //myMHZ19.calibrate();    // Take a reading which be used as the zero point for 400 ppm^
    myMHZ19.verify();
    _lastReadout = CO2Data();
#ifdef DEBUG
    Serial.println();
    Serial.println("MHZ19: OK");
#endif
}

void MHZ19Handler::printoutCurrentValues() {
    if(myMHZ19.errorCode == RESULT_OK) {
        Serial.println("MHZ19 CurrentValues:");
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
    else {
        Serial.println("Failed to recieve CO2 value - Error");
        Serial.print("Response Code: ");
        Serial.println(myMHZ19.errorCode);          // Get the Error Code value
    }
}


void MHZ19Handler::printoutLastReadout() {
        Serial.println("MHZ19 LastReadout:");
        Serial.println(name_MHZ19_co2             + ":          "   + String(_lastReadout.getRegular()       ));
        Serial.println(name_MHZ19_co2_raw         + ":       "      + String(_lastReadout.getRaw()           ));
        Serial.println(name_MHZ19_co2_limited     + ":            " + String(_lastReadout.getLimited()       ));
        Serial.println(name_MHZ19_co2_background  + ":         "    + String(_lastReadout.getBackground()    ));
        Serial.println(name_MHZ19_co2_tempAdjust  + ": "            + String(_lastReadout.getTempAdjustment()));
        Serial.println(name_MHZ19_co2_temperatur  + ":    "         + String(_lastReadout.getTemperature()   ));
        Serial.println(name_MHZ19_co2_Accuracy    + ":           "  + String(_lastReadout.getAccuracy()      ));
        Serial.println();
}

CO2Data MHZ19Handler::getLastReadout() {
    return _lastReadout;
}

bool MHZ19Handler::updateLastReadout() {
    if(myMHZ19.errorCode == RESULT_OK) {
        _lastReadout = CO2Data(
                myMHZ19.getCO2(),
                myMHZ19.getCO2Raw(),
                myMHZ19.getCO2(false),
                myMHZ19.getBackgroundCO2(),
                myMHZ19.getTempAdjustment(),
                myMHZ19.getTemperature(),
                myMHZ19.getAccuracy()
                );
        return true;
    }
    else {
        Serial.println("Failed to recieve CO2 value - Error");
        Serial.print("Response Code: ");
        Serial.println(myMHZ19.errorCode);          // Get the Error Code value
        return false;
    }
}

bool MHZ19Handler::runUpdate(const unsigned long currentSeconds) {
    if (currentSeconds % interval_MHZ19_in_Seconds == 0){
        return updateLastReadout();
    }
    return false;
}
