#ifdef M_DEVICE_FAKE_ECG

#include "modules/FakeECG.h"
#include <stdlib.h>

const int _refValues[] = {
    // 700 samples between peaks for 85-88 BPM
    330, 328, 327, 326, 325, 324, 323, 322, 321, 320, 319, 318, 317, 316, 315, 
    314, 313, 312, 311, 310, 309, 308, 307, 306, 305, 304, 303, 302, 301, 300, 
    299, 298, 297, 296, 295, 294, 293, 292, 291, 290, 289, 288, 287, 286, 285, 
    284, 283, 282, 281, 280, 279, 278, 277, 276, 275, 274, 273, 272, 271, 270,
    // P-wave
    330, 335, 340, 350, 370, 600, 400, 370, 365, 360, 355, 352, 350, 348, 346, 
    344, 342, 340, 338, 335, 332, 330, 328, 327, 326
};
int _refValueIndex = 0;
int fakeAnalogRead()
{
    int value = _refValues[_refValueIndex];
    _refValueIndex = (_refValueIndex + 1) % (sizeof(_refValues) / sizeof(_refValues[0]));
    return value;
}

void FakeECG::loop()
{
    BaseECG::loop();
}

void FakeECG::read()
{
    electrodeReadyForCallback = false;

    this->ecgAnalogOutput = fakeAnalogRead();

    if (this->ecgAnalogOutput > M_DEVICE_ECG_UPPER_THRESHOLD && !this->ignoreReading)
    {

        if (!this->firstPulseDetected)
        {
            this->firstPulseDetected = true;
            this->firstPulseTime = millis();
        }
        else
        {
            this->secondPulseTime = millis();
            this->pulseInterval = this->secondPulseTime - this->firstPulseTime;
            this->firstPulseTime = this->secondPulseTime;
        }
        this->ignoreReading = true;
    }

    if (this->ecgAnalogOutput < M_DEVICE_ECG_LOWER_THRESHOLD && this->ignoreReading)
    {
        this->ignoreReading = false;
    }

    this->rawReadyForCallback = true;
}

void FakeECG::reset()
{
    // this->bpm = 0.0;
    // this->isEcgDetached = true;
    // this->ignoreReading = false;
    // this->firstPulseDetected = false;
    // this->firstPulseTime = 0;
    // this->secondPulseTime = 0;
    // this->pulseInterval = 1;
}

void FakeECG::calculateBpm()
{
    if (this->pulseInterval == 0)
    {
        return;  // Ignore invalid pulse intervals
    }

    // Introduce slight variation (+/- 10ms)
    int variation = random(-10, 11);  // Random value between -10 and 10
    this->pulseInterval += variation;

    // Ensure pulse interval remains valid (avoid too large/small values)
    this->pulseInterval = max(600, min(this->pulseInterval, 900));

    // Calculate BPM with the adjusted interval
    this->bpm = (60.0 * 1000) / this->pulseInterval;

    this->bpmReadyForCallback = true;
}

#endif