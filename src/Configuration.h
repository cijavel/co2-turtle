#ifndef CO2_TURTLE_CONFIGURATION_H
#define CO2_TURTLE_CONFIGURATION_H

//uncomment to get debug in serial
#define DEBUG 0

#define switch_WIFI  1
#define switch_Webserver 1
#define switch_EPD 1
#define switch_LED 1
#define switch_MQTT 0
#define switch_EPD_orientation 0 // 0=vertical 90, 1=vertical 270, 2=horizontal 90, 3=horizontal 270

#define interval_MHZ19_in_Seconds 30
#define interval_BME680_in_Seconds 30
#define interval_WiFiCheck_in_Seconds 300
#define interval_RAMPrintout_in_Seconds 600
#define interval_EPD_in_Seconds 900
#define interval_LED_in_Seconds 30
#define interval_mqtt_in_Seconds 30

#define SEALEVELPRESSURE_HPA 1015
#define TEMPERATUR_OFFSET -3.5

#define DeviceName "Turtle 08.03.2026"
#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"

#define PIN_BME680_SDA 21
#define PIN_BME680_SCL 22
#define PIN_MHZ19_RX 17
#define PIN_MHZ19_TX 16
#define BAUDRATE 9600

#define BRIGHTNESS_LEDS 2 // range 2-255
#define NUM_LEDS 38
#define DATA_LED_PIN 4

#define MAX_POWER_MILLIAMPS 500
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

#define MQTT_PORT 1883
#define MQTT_USER_ENABLED 0


extern const char* DeviceNameConf;

// EPD hardware pins – set to -1 if not wired
#define EPD_PIN_RST  26
#define EPD_PIN_BUSY 25
#define EPD_PIN_DC 27
#define EPD_PIN_CS 5

#endif 