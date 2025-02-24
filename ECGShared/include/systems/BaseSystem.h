#ifndef BASE_SYSTEM_H
#define BASE_SYSTEM_H

#include <vector>
#include <algorithm>
#include <Stream.h>

#include "messages/messages.h"
#include "types/SystemTypes.h"

class BaseDevice;
class BaseSystem
{
protected:
    Print *outPrint = nullptr;
    Stream *inStream = nullptr;
    std::vector<byte> supportedInCommands;
    std::vector<byte> supportedOutCommands;
    bool initialized;
    bool connected;
    bool canTransmit = false;
    uint8_t deviceId[6];
    virtual bool isAlive()
    {
        return false;
    }

public:
    BaseSystem() {}

    Print *getOutPrint() { return outPrint; }
    Stream *getInStream() { return inStream; }
    bool isSupportedInCommand(byte command) { return std::find(supportedInCommands.begin(), supportedInCommands.end(), command) != supportedInCommands.end(); }
    bool isSupportedOutCommand(byte command) { return std::find(supportedOutCommands.begin(), supportedOutCommands.end(), command) != supportedOutCommands.end(); }
    std::vector<byte> getSupportedInCommands() { return supportedInCommands; }
    std::vector<byte> getSupportedOutCommands() { return supportedOutCommands; }

    virtual bool isInitialized()
    {
        return initialized;
    }

    virtual void connect(uint8_t deviceId[6] = {0})
    {
        memcpy(this->deviceId, deviceId, 6);
        this->connected = true;
    }

    virtual void disconnect()
    {
        this->connected = false;
        this->canTransmit = false;
        memset(this->deviceId, 0, 6);
        this->getOutPrint()->flush();
    }

    bool isConnected()
    {
        return connected && isAlive();
    }

    void getConnectedDeviceId(uint8_t *targetArray)
    {
        memcpy(targetArray, deviceId, 6);
    }

    bool getCanTransmit() {
        return this->canTransmit;
    }

    void readyToTransmit() {
        this->canTransmit = true;
    }

    virtual bool isType(SystemType type) = 0;
    virtual SystemType getType() {
        return SystemType::NONE;
    }
    virtual void loop()
    {
        if (!isAlive() && connected)
            disconnect();
    }
};

#endif // BASE_SYSTEM_H