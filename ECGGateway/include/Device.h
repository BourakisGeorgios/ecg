#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>
#include <RTC.h>
#include <vector>

#include "config.h"
#include "BaseDevice.h"

#include "messages/messages.h"
#include "timer/InaccurateTimer.h"

class DeviceImpl : public BaseDevice
{
private:
    bool isClockCorrectlySet;
    bool initClock();
    InaccurateTimer *announceTimer;
protected:
    void onComHandshaked() override;
    MessageHandling handleMessage(BaseSystem *bus, CommandMessage *message) override;

public:
    void getDeviceId(uint8_t *deviceId) override;
    void init() override;
    void loop() override;
    bool isOk() override;
    void announce();
    DeviceImpl();
    ~DeviceImpl();
};

extern DeviceImpl Device;

#endif