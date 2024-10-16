#ifndef LORA_BUS_H
#define LORA_BUS_H

#include <Arduino.h>
#include <LoRa.h>
#include "types/LoRaTypes.h"
#include "streams/LoRaStream.h"
#include "systems/BaseSystem.h"
#include "sharedConfig.h"

class LoRaBus : public BaseSystem
{
private:
    LoRaBuSMode mode;
    LoRaStream *stream;
    LoRaClass *lora;
protected:
    bool isAlive() override
    {
        return true;
    }
public:
    LoRaBus(LoRaBuSMode mode) : BaseSystem()
    {
        std::initializer_list<byte> nodeCommands = {
            (byte)Command::IdentifyRequest,
            (byte)Command::InfoRequest,
            (byte)Command::TimeResponse,
            (byte)Command::Ping,
            (byte)Command::Pong,
        };

        std::initializer_list<byte> gatewayCommands = {
            (byte)Command::IdentifyResponse,
            (byte)Command::Connect,
            (byte)Command::InfoResponse,
            (byte)Command::TimeRequest,
            (byte)Command::EcgBpm,
            (byte)Command::Pong,
            (byte)Command::Ping,
        };

        if (mode == LoRaBuSMode::Node)
        {
            this->supportedInCommands.assign(nodeCommands);
            this->supportedOutCommands.assign(gatewayCommands);
        }
        else
        {
            this->supportedInCommands.assign(gatewayCommands);
            this->supportedOutCommands.assign(nodeCommands);
        }
        this->lora = &LoRa;
        this->stream = new LoRaStream(lora);
        this->mode = mode;
    }
    ~LoRaBus()
    {
        delete stream;
    }
    bool init();

    LoRaBuSMode getMode()
    {
        return mode;
    }

    bool isType(SystemType type) override {
        return type == SystemType::LORA;
    }
};

#endif // LORA_BUS_H