#ifndef BaseECG_H
#define BaseECG_H

#include <Arduino.h>
#include <functional>
#include "timer/Timer.h"
#include "config.h"

class BaseECG
{
protected:
    std::function<void(double, bool)> bpmCallbackFn;
    std::function<void(int)> rawCallbackFn;
    std::function<void(bool)> electrodeAttachedCallbackFn;

    BaseTimer *bpmTimer = nullptr;
    BaseTimer *readTimer = nullptr;
    uint16_t readingIntervalMs = 10;
    uint16_t calculationIntervalMs = 1000;

    double bpm = 0.0;
    bool bpmReadyForCallback = false;
    bool rawReadyForCallback = false;
    bool electrodeReadyForCallback = false;
    bool isEcgDetached = true;

    int ecgAnalogOutput = 0;

    virtual void read() = 0;
    virtual void reset() = 0;
    virtual void calculateBpm() = 0;

public:
    BaseECG(uint16_t readIntervalMs = M_DEVICE_ECG_DEFAULT_READING_INTERVAL_MS,
            uint16_t calculationIntervalMs = M_DEVICE_ECG_DEFAULT_CALCULATION_INTERVAL_MS)
    {
        this->readingIntervalMs = readingIntervalMs;
        this->calculationIntervalMs = calculationIntervalMs;
        this->readTimer = new Timer(readingIntervalMs, new Callback(this, &BaseECG::read));
        this->bpmTimer = new Timer(calculationIntervalMs, new Callback(this, &BaseECG::calculateBpm));
    }
    virtual void init()
    {
        this->readTimer->start();
        this->bpmTimer->start();
    }
    virtual void loop()
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

    void setBpmCallback(std::function<void(double, bool)> callbackFn)
    {
        this->bpmCallbackFn = callbackFn;
    }

    void setRawCallback(std::function<void(int)> callbackFn)
    {
        this->rawCallbackFn = callbackFn;
    }

    void setElectrodeAttachCallback(std::function<void(bool)> callbackFn)
    {
        this->electrodeAttachedCallbackFn = callbackFn;
    }
};

#endif // BaseECG_H
