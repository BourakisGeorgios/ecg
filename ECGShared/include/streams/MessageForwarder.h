#ifndef FORWARDING_PROXY_STREAM_H
#define FORWARDING_PROXY_STREAM_H

#include <Stream.h>
#include <vector>
#include "messages/messages.h"
#include "systems/BaseSystem.h"

class MessageForwarder : public Print
{
private:
    bool forced = false;
public:
    MessageForwarder(std::vector<BaseSystem *> &baseSystems) : baseSystems(baseSystems) {}
    MessageForwarder() : baseSystems() {}
    MessageForwarder(BaseSystem *system) : forced(true)
    {
        baseSystems = std::vector<BaseSystem *>({system});
    }
    ~MessageForwarder()
    {
        baseSystems.clear();
    }

    void addSystem(BaseSystem *baseSystem)
    {
        baseSystems.push_back(baseSystem);
    }

    void flush() override
    {
        for (auto baseSystem : baseSystems)
        {
            baseSystem->getOutPrint()->flush();
        }
    }

    size_t write(uint8_t data) override
    {
        size_t written = 0;
        for (auto baseSystem : baseSystems)
        {
            written += baseSystem->getOutPrint()->write(data);
        }
        return written;
    }

    size_t write(const uint8_t *buffer, size_t size) override
    {
        size_t written = 0;
        for (auto baseSystem : baseSystems)
        {
            written += baseSystem->getOutPrint()->write(buffer, size);
        }
        return written;
    }

    size_t write(CommandMessage &msg)
    {

        size_t written = 0;
        size_t headerSize = msg.messageSize - msg.payloadSize;
        IPayload *payload = msg.payload;
        msg.payload = 0;
        const uint8_t *headerData = reinterpret_cast<const uint8_t *>(&msg);
        for (auto baseSystem : baseSystems)
        {
            if (!(baseSystem->isConnected() || forced) || !baseSystem->isSupportedOutCommand((byte)msg.command))
            {
                continue;
            }

            baseSystem->getConnectedDeviceId(msg.targetDeviceId);

            auto print = baseSystem->getOutPrint();
            written += print->write(headerData, headerSize);
            if (payload != nullptr && msg.payloadSize > 0)
            {
                written += print->write(
                    reinterpret_cast<const uint8_t *>(payload) + 4,
                    msg.payloadSize);
            }
        }
        return written;
    }

private:
    std::vector<BaseSystem *> baseSystems;
};

#endif