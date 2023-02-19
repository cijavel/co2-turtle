#ifndef _GxBitmaps3c128x250_H_
#define _GxBitmaps3c128x250_H_

#if defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif


// 'grad', 24x24px
const unsigned char bitmap_grad [] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xc9, 
	0xfe, 0x0f, 0x9c, 0xf8, 0x03, 0xbe, 0xf9, 0xf9, 0x9c, 0xf3, 0xf9, 0xc9, 0xf7, 0xff, 0xe3, 0xe7, 
	0xff, 0xff, 0xe7, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xf3, 0xf9, 
	0xff, 0xf9, 0xf9, 0xff, 0xf8, 0x03, 0xff, 0xfe, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};
// 'water-percent', 24x24pxr bitmap_temp
const unsigned char bitmap_hum [] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xff, 0xff, 
	0xc3, 0xff, 0xff, 0x81, 0xff, 0xff, 0x81, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xfe, 0x62, 
	0x7f, 0xfe, 0x66, 0x7f, 0xfc, 0x0c, 0x3f, 0xfc, 0x18, 0x3f, 0xfc, 0x30, 0x3f, 0xfc, 0x66, 0x3f, 
	0xfe, 0x46, 0x7f, 0xfe, 0x00, 0x7f, 0xff, 0x00, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};
// 'CO', 24x24px
const unsigned char bitmap_CO2 [] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xfe, 
	0x00, 0x7f, 0xfc, 0x7e, 0x3f, 0xe0, 0xff, 0x3f, 0xc1, 0xff, 0x9f, 0x9f, 0xff, 0x87, 0x9c, 0x61, 
	0xc1, 0x3b, 0xad, 0xf9, 0x3b, 0xed, 0x3c, 0x3b, 0xed, 0xbc, 0x3b, 0xad, 0x7c, 0x9c, 0x61, 0x3c, 
	0x8f, 0xff, 0xf9, 0xc0, 0x00, 0x01, 0xe0, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};
// 'heat-wave', 24x24px
const unsigned char bitmap_aiq [] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xf8, 0x7f, 0xf1, 
	0xe3, 0x1f, 0xfc, 0xcf, 0xcf, 0xfe, 0x1f, 0xe3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xf8, 
	0x7f, 0xf3, 0xf3, 0x3f, 0xf8, 0xc7, 0x8f, 0xfe, 0x1f, 0xe3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xc7, 0xf8, 0x7f, 0xf1, 0xe3, 0x1f, 0xfc, 0xcf, 0xcf, 0xfe, 0x1f, 0xe3, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};
// 'thermometer', 24x24px
const unsigned char bitmap_temp [] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 0x81, 0xff, 0xff, 0x99, 0xff, 0xff, 
	0x99, 0xff, 0xff, 0x99, 0xff, 0xff, 0x99, 0xff, 0xff, 0x81, 0xff, 0xff, 0x81, 0xff, 0xff, 0x81, 
	0xff, 0xff, 0x81, 0xff, 0xff, 0x81, 0xff, 0xff, 0x81, 0xff, 0xff, 0x00, 0xff, 0xfe, 0x00, 0x7f, 
	0xfe, 0x00, 0x7f, 0xfe, 0x00, 0x7f, 0xfe, 0x00, 0x7f, 0xff, 0x00, 0xff, 0xff, 0x81, 0xff, 0xff, 
	0xc3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 480)
const int bitmap_allArray_LEN = 5;
const unsigned char* bitmap_allArray[5] = {
	bitmap_CO2,
	bitmap_grad,
	bitmap_aiq,
	bitmap_temp,
	bitmap_hum
};




#endif
