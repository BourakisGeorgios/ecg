#include "modules/ECG.h"

void ECG::loop()
{
    BaseECG::loop();
}

void ECG::read()
{
    PinStatus ecgLoPlus = digitalRead(this->digitalLoPlusPinIn);
    PinStatus ecgLoMinus = digitalRead(this->digitalLoMinusPinIn);
    electrodeReadyForCallback = false;

    // if (ecgLoMinus == HIGH || ecgLoPlus == HIGH)
    // {
    //     if (!this->isEcgDetached)
    //         this->electrodeReadyForCallback = true;
    //     this->isEcgDetached = true;
    //     this->reset();
    // }
    // else
    // {
    //     if (this->isEcgDetached)
    //         this->electrodeReadyForCallback = true;
    //     this->isEcgDetached = false;
    // }

    // if (!this->isEcgDetached)
    // {
    this->ecgAnalogOutput = analogRead(this->analogPinOut);

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

void ECG::reset()
{
    // this->bpm = 0.0;
    // this->isEcgDetached = true;
    // this->ignoreReading = false;
    // this->firstPulseDetected = false;
    // this->firstPulseTime = 0;
    // this->secondPulseTime = 0;
    // this->pulseInterval = 1;
}

void ECG::calculateBpm()
{
    if (this->pulseInterval == 0)
    {
        return;
    }

    this->bpm = (1.0 / this->pulseInterval) * M_DEVICE_ECG_MINUTE_SECONDS * M_DEVICE_ECG_SECOND_MILLIS;

    this->bpmReadyForCallback = true;
}
