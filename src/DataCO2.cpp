#include "DataCO2.h"

int DataCO2::getRegular() const {
    return regular;
}

void DataCO2::setRegular(int regular) {
    DataCO2::regular = regular;
}

unsigned int DataCO2::getRaw() const {
    return raw;
}

void DataCO2::setRaw(unsigned int raw) {
    DataCO2::raw = raw;
}

int DataCO2::getLimited() const {
    return limited;
}

void DataCO2::setLimited(int limited) {
    DataCO2::limited = limited;
}

int DataCO2::getBackground() const {
    return background;
}

void DataCO2::setBackground(int background) {
    DataCO2::background = background;
}

byte DataCO2::getTempAdjustment() const {
    return tempAdjustment;
}

void DataCO2::setTempAdjustment(byte tempAdjustment) {
    DataCO2::tempAdjustment = tempAdjustment;
}

float DataCO2::getTemperature() const {
    return temperature;
}

void DataCO2::setTemperature(float temperature) {
    DataCO2::temperature = temperature;
}

byte DataCO2::getAccuracy() const {
    return accuracy;
}

void DataCO2::setAccuracy(byte accuracy) {
    DataCO2::accuracy = accuracy;
}

DataCO2::DataCO2() {}

DataCO2::DataCO2(int regular, unsigned int raw, int limited, int background, byte tempAdjustment, float temperature,
                 byte accuracy) : regular(regular), raw(raw), limited(limited), background(background),
                                  tempAdjustment(tempAdjustment), temperature(temperature), accuracy(accuracy) {}