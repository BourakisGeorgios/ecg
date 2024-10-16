#ifndef ECG_H
#define ECG_H

#include <Arduino.h>
#include <functional>
#include "timer/Timer.h"
#include "config.h"

class ECG
{
private:
    uint8_t analogPinOut = 255;
    uint8_t digitalLoPlusPinIn = 255;
    uint8_t digitalLoMinusPinIn = 255;
    uint16_t readingIntervalMs = 10;
    uint16_t calculationIntervalMs = 1000;
    std::function<void(double,bool)> bpmCallbackFn;
    std::function<void(int)> rawCallbackFn;
    std::function<void(bool)> electrodeAttachedCallbackFn;

    double bpm = 0.0;
    bool bpmReadyForCallback = false;
    bool rawReadyForCallback = false;
    bool electrodeReadyForCallback = false;
    bool isEcgDetached = true;
    bool ignoreReading = false;
    bool firstPulseDetected = false;
    unsigned long firstPulseTime = 0;
    unsigned long secondPulseTime = 0;
    unsigned long pulseInterval = 0;
    int beatOld = 0;
    int beats[500] = {0};
    int beatIndex = 0;

    int ecgAnalogOutput = 0;

    void read();
    void reset();
    void calculateBpm();
    
    BaseTimer *bpmTimer = nullptr;
    BaseTimer *readTimer = nullptr;

public:
    ECG(
        uint8_t analogPinOut = M_DEVICE_ECG_ANALOG_OUT,
        uint8_t digitalLoPlusPinIn = M_DEVICE_ECG_LOW_PLUS_IN,
        uint8_t digitalLoMinusPinIn = M_DEVICE_ECG_LOW_MINUS_IN,
        uint16_t readIntervalMs = M_DEVICE_ECG_DEFAULT_READING_INTERVAL_MS,
        uint16_t calculationIntervalMs = M_DEVICE_ECG_DEFAULT_CALCULATION_INTERVAL_MS);
    void init();
    void loop();

    void setBpmCallback(std::function<void(double,bool)> callbackFn);
    void setRawCallback(std::function<void(int)> callbackFn);
    void setElectrodeAttachCallback(std::function<void(bool)> callbackFn);
};

#endif // ECG_H
