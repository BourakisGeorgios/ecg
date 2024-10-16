#ifndef MESSAGE_BUS_H
#define MESSAGE_BUS_H

#include <Arduino.h>
#include <sdios.h>
#include <StreamUtils.h>

#include "messages/messages.h"
#include "streams/NullStream.h"
#include "systems/BaseSystem.h"

class BaseDevice;

class MessageBus
{
private:
    BaseDevice *device = nullptr;
    BaseSystem* system = nullptr;
    static const int overflowBytesWait = 256;
    bool commandMessageReceived = false;
    bool recvInProgress = false;
    bool hasCommandReceived = false;
    Command commandReceived;
    bool hasSizeReceived = false;
    size_t sizeReceived = 0;
    bool hasOriginDeviceIdReceived = false;
    byte originDeviceIdReceived[6] = {0,0,0,0,0,0};
    bool hasTargetDeviceIdReceived = false;
    byte targetDeviceIdReceived[6] = {0,0,0,0,0,0};
    bool hasTimeReceived = false;
    uint32_t timeReceived = 0;
    bool hasRtcTimeReceived = false;
    uint32_t rtcTimeReceived = 0;
    bool hasPayloadSizeReceived = false;
    size_t payloadSizeReceived = 0;
    int midx = 0;
    byte *messageBuffer = nullptr;
    int bytesRead = 0;
    int remainingBytes = 0;
    byte *payloadBuffer = nullptr;

    CommandMessage *parseCommand();
    void clear();
    bool readHeaders();
    void completeConsume();
    bool isMessageReceivedCompleted()
    {
        return hasCommandReceived && commandMessageReceived && hasSizeReceived && hasTimeReceived && hasRtcTimeReceived && hasPayloadSizeReceived;
    }
    CommandMessage *recv();
    template <typename T>
    bool readBytes(T *dest, size_t size);
    bool readSingle(byte *byte);

public:
    MessageBus(BaseDevice *device, BaseSystem* system) : device(device), system(system)
    {
    }
    void loop();
    BaseSystem* getSystem() {
        return system;
    }
};

#endif // MESSAGE_BUS_H