#ifdef M_DEVICE_FAKE_ECG

#include "modules/FakeECG.h"
#include <stdlib.h>

int fakeAnalogRead() {
    return 0;
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
    int num = rand() % 2;

    this->bpm = 78 + num;

    this->bpmReadyForCallback = true;
}

#endif