#ifndef BASE_DEVICE_H
#define BASE_DEVICE_H

#include <Arduino.h>
#include "streams/MessageForwarder.h"
#include "messaging/MessageBus.h"
#include "systems/COM.h"
#include "systems/LoRaBuS.h"

class BaseDevice
{
private:
    std::vector<MessageBus *> messageBuses;
    MessageForwarder *messageForwarder;

protected:
    COM *com;
    LoRaBus *lora;

protected:
    bool initSerial();
    bool initLoRa();
    void registerSystem(BaseSystem *system);
    std::vector<MessageBus *> getMessageBuses();

public:
    virtual bool isOk();
    virtual void processMessage(BaseSystem *bus, CommandMessage *message) = 0;
    virtual void init();
    virtual void loop();
    void dispatchMessage(CommandMessage *message);
    void dispatchMessage(BaseSystem *bus, CommandMessage *message);
    BaseDevice(LoRaBuSMode loraBusMode = LoRaBuSMode::Node);
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