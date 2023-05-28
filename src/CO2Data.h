#ifndef CO2_TURTLE_CO2DATA_H
#define CO2_TURTLE_CO2DATA_H

#include <Arduino.h>

class CO2Data {
private:
    int regular{};
    unsigned int raw{};
    int limited{};
    int background{};
    byte tempAdjustment{};
    float temperature{};
    byte accuracy{};
public:
    CO2Data(int regular, unsigned int raw, int limited, int background, byte tempAdjustment, float temperature,
            byte accuracy);
    CO2Data();

    int getRegular() const;
    void setRegular(int regular);
    unsigned int getRaw() const;
    void setRaw(unsigned int raw);
    int getLimited() const;
    void setLimited(int limited);
    int getBackground() const;
    void setBackground(int background);
    byte getTempAdjustment() const;
    void setTempAdjustment(byte tempAdjustment);
    float getTemperature() const;
    void setTemperature(float temperature);
    byte getAccuracy() const;
    void setAccuracy(byte accuracy);
};
#endif //CO2_TURTLE_CO2DATA_H
