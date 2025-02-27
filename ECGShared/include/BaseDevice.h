#ifndef BASE_DEVICE_H
#define BASE_DEVICE_H

#include <Arduino.h>
#include <RTC.h>
#include <unordered_map>

#include "messaging/MessageBus.h"
#include "messages/DeviceInfo.h"
#include "messages/DeviceType.h"
#include "streams/MessageForwarder.h"
#include "systems/COM.h"
#include "systems/LoRaBuS.h"
#include "types/MessageHandlingTypes.h"
#include "types/StorageTypes.h"

class BaseDevice
{
private:
    std::vector<MessageBus *> messageBuses;
    MessageForwarder *messageForwarder;
    std::unordered_map<BaseSystem *, bool> systemHandshakeMap = {};

    MessageHandling handleHandshake(BaseSystem *system, CommandMessage *message);
    bool initSerial();
    bool initLoRa();

protected:
    COM *com;
    LoRaBus *lora;
    DeviceInfo deviceInfo;

    void registerSystem(BaseSystem *system);
    std::vector<MessageBus *> getMessageBuses();
    virtual MessageHandling handleMessage(BaseSystem *bus, CommandMessage *message) = 0;
    virtual StorageState getStorageState();
    virtual void onComHandshaked() {
        
    }
    void dispatchMessage(BaseSystem *system, CommandMessage *message, bool safe);
    void dispatchMessage(CommandMessage *message, bool safe);

public:
    virtual void getDeviceId(uint8_t *deviceId) = 0;
    virtual bool isOk();
    virtual void init();
    virtual void loop();
    void processMessage(BaseSystem *bus, CommandMessage *message);
    void dispatchMessage(BaseSystem *bus, CommandMessage *message);
    void dispatchMessage(CommandMessage *message);
    bool isHandshaked(BaseSystem *system);
    DeviceInfo& getDeviceInfo() {
        return deviceInfo;
    }

    BaseDevice(DeviceInfo info, LoRaBuSMode loraBusMode = LoRaBuSMode::Node);
    virtual ~BaseDevice()
    {
        delete messageForwarder;
        delete com;
        for (auto messageBus : messageBuses)
        {
            delete messageBus;
        }
    };
};

#endif // BASE_DEVICE_H