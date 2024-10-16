#include "modules/ECG.h"

ECG::ECG(
    uint8_t analogPinOut,
    uint8_t digitalLoPlusPinIn,
    uint8_t digitalLoMinusPinIn,
    uint16_t readingIntervalMs,
    uint16_t calculationIntervalMs)
{
    this->analogPinOut = analogPinOut;
    this->digitalLoMinusPinIn = digitalLoMinusPinIn;
    this->digitalLoPlusPinIn = digitalLoMinusPinIn;
    this->readingIntervalMs = readingIntervalMs;
    this->calculationIntervalMs = calculationIntervalMs;
    this->readTimer = new Timer(readingIntervalMs, new Callback(this, &ECG::read));
    this->bpmTimer = new Timer(calculationIntervalMs, new Callback(this, &ECG::calculateBpm));
    pinMode(this->analogPinOut, INPUT);
    pinMode(this->digitalLoMinusPinIn, INPUT);
    pinMode(this->digitalLoPlusPinIn, INPUT);
}

void ECG::init()
{
    this->readTimer->start();
    this->bpmTimer->start();
}

void ECG::loop()
{
    if (this->bpmReadyForCallback)
    {
        if (this->bpmCallbackFn != nullptr)
        {
            this->bpmCallbackFn(this->bpm, !this->isEcgDetached);
        }
        this->bpmReadyForCallback = false;
    }
    if (this->rawReadyForCallback)
    {
        if (this->rawCallbackFn != nullptr)
        {
            this->rawCallbackFn(this->ecgAnalogOutput);
        }
        this->rawReadyForCallback = false;
    }
    if (this->electrodeReadyForCallback)
    {
        if (this->electrodeAttachedCallbackFn != nullptr)
        {
            this->electrodeAttachedCallbackFn(!this->isEcgDetached);
        }
        this->electrodeReadyForCallback = false;
    }
    this->readTimer->loop();
    this->bpmTimer->loop();
}

void ECG::setBpmCallback(std::function<void(double, bool)> callbackFn)
{
    this->bpmCallbackFn = callbackFn;
}

void ECG::setRawCallback(std::function<void(int)> callbackFn)
{
    this->rawCallbackFn = callbackFn;
}

void ECG::setElectrodeAttachCallback(std::function<void(bool)> callbackFn)
{
    this->electrodeAttachedCallbackFn = callbackFn;
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
