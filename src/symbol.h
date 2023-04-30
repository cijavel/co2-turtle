#ifndef _GxBitmaps3c128x250_H_
#define _GxBitmaps3c128x250_H_

#if defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif

// 'ohm', 10x10px
const unsigned char bitmap_ohm13 [] PROGMEM = {
	0xff, 0xc0, 0xe1, 0xc0, 0xde, 0xc0, 0xbf, 0x40, 0xbf, 0x40, 0xbf, 0x40, 0xbf, 0x40, 0x9e, 0x40, 
	0xcc, 0xc0, 0x8c, 0x40
};
// 'ohm14', 12x12px
const unsigned char bitmap_ohm14 [] PROGMEM = {
	0xff, 0xf0, 0xe0, 0xf0, 0xce, 0x70, 0x9f, 0x30, 0xbf, 0xb0, 0xbf, 0xb0, 0xbf, 0xb0, 0x9f, 0x30, 
	0xdf, 0x70, 0xee, 0xf0, 0x8e, 0x30, 0xff, 0xf0
};
// 'grad', 24x24px
const unsigned char bitmap_grad [] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xc9, 
	0xfe, 0x0f, 0x9c, 0xf8, 0x03, 0xbe, 0xf9, 0xf9, 0x9c, 0xf3, 0xf9, 0xc9, 0xf7, 0xff, 0xe3, 0xe7, 
	0xff, 0xff, 0xe7, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xf3, 0xf9, 
	0xff, 0xf9, 0xf9, 0xff, 0xf8, 0x03, 0xff, 0xfe, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// 'grad18', 18x18px
const unsigned char bitmap_grad18 [] PROGMEM = {
	0xff, 0xff, 0xc0, 0x8f, 0x87, 0xc0, 0x76, 0x73, 0xc0, 0x76, 0xfb, 0xc0, 0x74, 0xfd, 0xc0, 0x8d, 
	0xff, 0xc0, 0xfd, 0xff, 0xc0, 0xfd, 0xff, 0xc0, 0xfc, 0xfd, 0xc0, 0xfe, 0xfb, 0xc0, 0xfe, 0x73, 
	0xc0, 0xff, 0x87, 0xc0, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xc0, 
	0xff, 0xff, 0xc0, 0xff, 0xff, 0xc0
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

// 'ppm18', 18x18px
const unsigned char bitmap_ppm18 [] PROGMEM = {
	0x8c, 0x7f, 0xc0, 0xb5, 0xbf, 0xc0, 0xb5, 0xbf, 0xc0, 0xb5, 0xbf, 0xc0, 0x8c, 0x7f, 0xc0, 0xbd, 
	0xff, 0xc0, 0xbd, 0xff, 0xc0, 0xbd, 0xff, 0xc0, 0xff, 0xff, 0xc0, 0xa2, 0x3f, 0xc0, 0x99, 0xbf, 
	0xc0, 0xbb, 0xbf, 0xc0, 0xbb, 0xbf, 0xc0, 0xbb, 0xbf, 0xc0, 0xbb, 0xbf, 0xc0, 0xbb, 0xbf, 0xc0, 
	0xff, 0xff, 0xc0, 0xff, 0xff, 0xc0
};

// 'iaq', 18x18px
const unsigned char bitmap_iaq [] PROGMEM = {
	0xff, 0xff, 0xc0, 0xbb, 0xc7, 0xc0, 0xbb, 0xbb, 0xc0, 0xbb, 0xbb, 0xc0, 0xb5, 0xbb, 0xc0, 0xb1, 
	0xab, 0xc0, 0xb5, 0xb7, 0xc0, 0xae, 0xcb, 0xc0, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xc0, 0xff, 0xff, 
	0xc0, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xc0, 
	0xff, 0xff, 0xc0, 0xff, 0xff, 0xc0
};

// 'turtle', 18x18px
const unsigned char bitmap_turtle [] PROGMEM = {
	0xff, 0xff, 0xc0, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xc0, 0xe0, 0x70, 0xc0, 0xdf, 
	0xaf, 0x40, 0xbf, 0xcd, 0x40, 0x7f, 0xef, 0x40, 0x7f, 0xec, 0xc0, 0x7f, 0xcd, 0xc0, 0x80, 0x3d, 
	0xc0, 0xbf, 0xe3, 0xc0, 0xb3, 0x97, 0xc0, 0xad, 0x57, 0xc0, 0x8c, 0x47, 0xc0, 0xff, 0xff, 0xc0, 
	0xff, 0xff, 0xc0, 0xff, 0xff, 0xc0
};

// 'turtlesleep', 18x18px
const unsigned char bitmap_turtlesleep [] PROGMEM = {
	0xff, 0xf8, 0x40, 0xff, 0xfe, 0xc0, 0xff, 0xfd, 0xc0, 0xff, 0x88, 0x40, 0xff, 0xdf, 0xc0, 0xff, 
	0x8f, 0xc0, 0xff, 0xff, 0xc0, 0xe0, 0x7f, 0xc0, 0xdf, 0xbf, 0xc0, 0xbf, 0xcf, 0xc0, 0x7f, 0xef, 
	0xc0, 0x7f, 0xef, 0xc0, 0x7f, 0xcf, 0xc0, 0x80, 0x3f, 0xc0, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xc0, 
	0xff, 0xff, 0xc0, 0xff, 0xff, 0xc0
};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 480)
const int bitmap_allArray_LEN = 12;
const unsigned char* bitmap_allArray[12] = {
	bitmap_CO2,
	bitmap_grad,
	bitmap_grad18,
	bitmap_aiq,
	bitmap_iaq,
	bitmap_temp,
	bitmap_hum,
	bitmap_ohm13,
	bitmap_ohm14,
	bitmap_ppm18,
	bitmap_turtle,
	bitmap_turtlesleep
};




#endif