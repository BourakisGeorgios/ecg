#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>
#include <TimeLib.h>
#include <vector>

#include "config.h"
#include "BaseDevice.h"

#include "messages/messages.h"

class DeviceImpl : public BaseDevice
{
private:
public:
    void init() override;
    void loop() override;
    void processMessage(BaseSystem *bus, CommandMessage *message) override;
    bool isOk() override;
    DeviceImpl();
    ~DeviceImpl();
};

extern DeviceImpl Device;

#endif