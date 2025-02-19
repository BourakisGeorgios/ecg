#ifndef ECG_H
#define ECG_H

#include "BaseECG.h"

class ECG : public BaseECG
{
private:
    uint8_t analogPinOut = 255;
    uint8_t digitalLoPlusPinIn = 255;
    uint8_t digitalLoMinusPinIn = 255;

    bool ignoreReading = false;
    bool firstPulseDetected = false;
    unsigned long firstPulseTime = 0;
    unsigned long secondPulseTime = 0;
    unsigned long pulseInterval = 0;
    int beatOld = 0;
    int beats[500] = {0};
    int beatIndex = 0;

protected:
    virtual void read();
    virtual void reset();
    virtual void calculateBpm();

public:
    ECG(
        uint8_t analogPinOut = M_DEVICE_ECG_ANALOG_OUT,
        uint8_t digitalLoPlusPinIn = M_DEVICE_ECG_LOW_PLUS_IN,
        uint8_t digitalLoMinusPinIn = M_DEVICE_ECG_LOW_MINUS_IN,
        uint16_t readIntervalMs = M_DEVICE_ECG_DEFAULT_READING_INTERVAL_MS,
        uint16_t calculationIntervalMs = M_DEVICE_ECG_DEFAULT_CALCULATION_INTERVAL_MS
    ) : BaseECG(readingIntervalMs, calculationIntervalMs)
    {
        this->analogPinOut = analogPinOut;
        this->digitalLoMinusPinIn = digitalLoMinusPinIn;
        this->digitalLoPlusPinIn = digitalLoMinusPinIn;
        pinMode(this->analogPinOut, INPUT);
        pinMode(this->digitalLoMinusPinIn, INPUT);
        pinMode(this->digitalLoPlusPinIn, INPUT);
    }
    virtual void loop();
};

#endif // ECG_H
