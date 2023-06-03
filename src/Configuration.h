#ifndef CO2_TURTLE_CONFIGURATION_H
#define CO2_TURTLE_CONFIGURATION_H

//uncomment to get debug in serial
#define DEBUG

#define interval_MHZ19_in_Seconds 30
#define interval_BME680_in_Seconds 30
#define interval_WiFiCheck_in_Seconds 60
#define interval_RAMPrintout_in_Seconds 30
#define interval_EPD_in_Seconds 60

#define PIN_BME680_SDA 21
#define PIN_BME680_SCL 22
#define PIN_MHZ19_RX 17
#define PIN_MHZ19_TX 16
#define BAUDRATE 9600
#define DeviceName "SensorTurtle 1"


// leds
#define NUM_LEDS 34 //count
#define PIN_LED_DATA 4
#define BRIGHTNESS_LEDS 50

#endif //CO2_TURTLE_CONFIGURATION_H
