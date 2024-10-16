#ifndef LORA_BUS_H
#define LORA_BUS_H

#include <Arduino.h>
#include <LoRa.h>
#include <RTC.h>
#include "types/LoRaTypes.h"
#include "streams/LoRaStream.h"
#include "systems/BaseSystem.h"
#include "timer/InaccurateTimer.h"
#include "sharedConfig.h"

class BaseDevice;
class LoRaBus : public BaseSystem
{
private:
    LoRaBuSMode mode;
    LoRaStream *stream;
    LoRaClass *lora;
    BaseTimer *discoverTimer;
    BaseDevice* device;

    void broadcastDiscover();
protected:
    bool isAlive() override
    {
        return true;
    }
public:
    LoRaBus(BaseDevice* device, LoRaBuSMode mode) : BaseSystem()
    {
        std::initializer_list<byte> nodeCommands = {
            (byte)Command::IdentifyRequest,
            (byte)Command::InfoRequest,
            (byte)Command::TimeResponse,
            (byte)Command::Discover,
            (byte)Command::Connect,
            (byte)Command::Ping,
            (byte)Command::Pong,
        };

        std::initializer_list<byte> gatewayCommands = {
            (byte)Command::IdentifyResponse,
            (byte)Command::Connect,
            (byte)Command::Discover,
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
        this->discoverTimer = new InaccurateTimer(1000, new Callback(this, &LoRaBus::broadcastDiscover));
        this->device = device;
    }
    ~LoRaBus()
    {
        delete stream;
    }
    bool init();
    void loop();

    LoRaBuSMode getMode()
    {
        return mode;
    }

    bool isType(SystemType type) override {
        return type == SystemType::LORA;
    }
};

#endif // LORA_BUS_H