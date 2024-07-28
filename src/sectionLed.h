#ifndef SECTIONLED_H
#define SECTIONLED_H

// Define an enumeration for section names
enum SectionName {
    LED_WLANCONNECT,
    LED_STATUS,
    LED_TEMP,
    LED_HUM,
    LED_AIRQ,
    LED_CO2,
    NUM_SECTIONS
};

// Define the sections (start and end indices)
struct SectionStruc {
    int startLed;
    int endLed;
};

#endif // SECTIONLED_H