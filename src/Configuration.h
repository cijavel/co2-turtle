#ifndef CO2_TURTLE_CONFIGURATION_H
#define CO2_TURTLE_CONFIGURATION_H

//uncomment to get debug in serial
#define DEBUG 0

#define interval_MHZ19_in_Seconds 60
#define interval_BME680_in_Seconds 60
#define interval_WiFiCheck_in_Seconds 120
#define interval_RAMPrintout_in_Seconds 600
#define interval_EPD_in_Seconds 900
#define interval_LED_in_Seconds 60

#define PIN_BME680_SDA 21
#define PIN_BME680_SCL 22
#define PIN_MHZ19_RX 17
#define PIN_MHZ19_TX 16
#define BAUDRATE 9600
#define DeviceName "SensorTurtle 1"

#define SEALEVELPRESSURE_HPA 1015
#define BRIGHTNESS_LEDS 10
#define NUM_LEDS 34
#define DATA_LED_PIN 4

#define MAX_POWER_MILLIAMPS 500
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

#endif //CO2_TURTLE_CONFIGURATION_H
