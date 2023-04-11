#include <Arduino.h>

// _______________
// EPD ePaper eINK
// ---------------
// BUSY 	  -> -1			Violett, 
// RST 	  	-> -1 		RX2	Blau, 
// DC 		  -> 27  		TX2	grün, 
// CS 		  -> SS(5)		gelb, 
// CLK 		  -> SCK(18)		orange, 
// DIN /SDI -> MOSI(23) 	weiß, 
// GND 		  -> GND, 
// 3.3V 	  -> 3.3V
//#define GxEPD2_DRIVER_CLASS GxEPD2_213_Z98c // GDEY0213Z98 122x250, SSD1680


// _______________
// LED
// ---------------
// DATA -> 4

// _______________
// MH-Z19B
// ---------------
// GND -> GND
// VCC -> 5V
// RX  -> 17  
// TX  -> 16 

// _______________
// Bosch BME680
// ---------------
// GND -> GND
// VCC -> 5V
// SCL -> 22  
// SDA -> 21  
// SD0 -> -
// CS  -> -