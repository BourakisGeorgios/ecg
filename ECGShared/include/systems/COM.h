#ifndef COM_H
#define COM_H

#include <Arduino.h>

#include "systems/BaseSystem.h"
#include "streams/MessageForwarder.h"

class COM : public BaseSystem
{
protected:
    bool isAlive() override
    {
        return true;
    }

public:
    COM()
    {
        this->supportedInCommands
            .assign({(byte)Command::IdentifyRequest,
                     (byte)Command::InfoRequest,
                     (byte)Command::TimeResponse});
        this->supportedOutCommands
            .assign({(byte)Command::IdentifyResponse,
                     (byte)Command::InfoResponse,
                     (byte)Command::TimeRequest,
                     (byte)Command::EcgBpm,
                     (byte)Command::EcgElectrodeAttached,
                     (byte)Command::EcgRaw,
                     (byte)Command::Log,
                     (byte)Command::Discover,
                     (byte)Command::Debug});
    }
    bool init();

    bool isType(SystemType type) override
    {
        return type == SystemType::COM;
    }
    SystemType getType() override {
        return SystemType::COM;
    }
};

#endif