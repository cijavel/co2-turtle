#include "CO2Data.h"

int CO2Data::getRegular() const {
    return regular;
}

void CO2Data::setRegular(int regular) {
    CO2Data::regular = regular;
}

unsigned int CO2Data::getRaw() const {
    return raw;
}

void CO2Data::setRaw(unsigned int raw) {
    CO2Data::raw = raw;
}

int CO2Data::getLimited() const {
    return limited;
}

void CO2Data::setLimited(int limited) {
    CO2Data::limited = limited;
}

int CO2Data::getBackground() const {
    return background;
}

void CO2Data::setBackground(int background) {
    CO2Data::background = background;
}

byte CO2Data::getTempAdjustment() const {
    return tempAdjustment;
}

void CO2Data::setTempAdjustment(byte tempAdjustment) {
    CO2Data::tempAdjustment = tempAdjustment;
}

float CO2Data::getTemperature() const {
    return temperature;
}

void CO2Data::setTemperature(float temperature) {
    CO2Data::temperature = temperature;
}

byte CO2Data::getAccuracy() const {
    return accuracy;
}

void CO2Data::setAccuracy(byte accuracy) {
    CO2Data::accuracy = accuracy;
}

CO2Data::CO2Data() {}

CO2Data::CO2Data(int regular, unsigned int raw, int limited, int background, byte tempAdjustment, float temperature,
                 byte accuracy) : regular(regular), raw(raw), limited(limited), background(background),
                                  tempAdjustment(tempAdjustment), temperature(temperature), accuracy(accuracy) {}