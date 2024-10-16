#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>
#include <RTC.h>
#include <vector>

#include "config.h"
#include "BaseDevice.h"

#include "systems/Storage.h"
#include "messages/messages.h"

class DeviceImpl : public BaseDevice
{
private:
    Storage *storage;
    bool isClockCorrectlySet;
    bool initClock();
    bool initStorage();
protected:
    MessageHandling handleMessage(BaseSystem *bus, CommandMessage *message) override;
    StorageState getStorageState() override;
public:
    void getDeviceId(uint8_t *deviceId) override;
    void init() override;
    void loop() override;
    bool isOk() override;
    DeviceImpl();
    ~DeviceImpl();
};

extern DeviceImpl Device;

#endif