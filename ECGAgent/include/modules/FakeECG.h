#ifndef FakeECG_H
#define FakeECG_H

#ifdef M_DEVICE_FAKE_ECG
#include "BaseECG.h"

class FakeECG : public BaseECG
{
private:
    bool ignoreReading = false;
    bool firstPulseDetected = false;
    unsigned long firstPulseTime = 0;
    unsigned long secondPulseTime = 0;
    unsigned long pulseInterval = 0;
    int beatOld = 0;
    int beats[500] = {0};
    int beatIndex = 0;

    int ecgAnalogOutput = 0;

protected:
    virtual void read();
    virtual void reset();
    virtual void calculateBpm();

public:
    FakeECG(
        uint16_t readIntervalMs = M_DEVICE_ECG_DEFAULT_READING_INTERVAL_MS,
        uint16_t calculationIntervalMs = M_DEVICE_ECG_DEFAULT_CALCULATION_INTERVAL_MS
    ) : BaseECG(readingIntervalMs, calculationIntervalMs)
    {
    }
    virtual void loop();
};

#endif
#endif // FakeECG_H