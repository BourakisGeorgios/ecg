#include "Device.h"

bool DeviceImpl::isOk()
{
    return com->isInitialized();
}

void DeviceImpl::init()
{
    BaseDevice::init();
}

void DeviceImpl::processMessage(BaseSystem *system, CommandMessage *message)
{
    BaseSystem *targetSystem;
    if (system->isType(SystemType::COM))
    {
        targetSystem = this->lora;
    }
    else if (system->isType(SystemType::LORA))
    {
        targetSystem = this->com;
    }
    else
    {
        while (1)
            ;
    }

    switch (message->command)
    {
    case Command::IdentifyRequest:
    {
        auto response = createMessage(Command::IdentifyResponse, now(), new IdentifyPayload());
        dispatchMessage(system, response);
        system->connect();
        auto timeRequest = createMessage(Command::TimeRequest, now());
        dispatchMessage(system, timeRequest);
        break;
    }
    case Command::TimeResponse:
    {
        TimePayload *payload = (TimePayload *)message->payload;
        setTime(payload->data);
        // auto response = createMessage(Command::IdentifyResponse, time, new IdentifyPayload());
        // dispatchMessage(system, response);
        break;
    }
    case Command::InfoRequest:
    {
        auto *infoPayload = new InfoPayload();
        infoPayload->com = COMInfoPayload{
            .initialized = com->isInitialized(),
            .connected = com->isConnected(),
            .isCurrent = system->isType(SystemType::COM),
        };
        infoPayload->connection = ConnectionInfoPayload{
            .initialized = lora->isInitialized(),
            .connected = lora->isConnected(),
            .isCurrent = system->isType(SystemType::LORA),
            .mode = lora->getMode(),
        };

        infoPayload->storage = StorageInfoPayload{
            .initialized = false,
            .state = {
                .status = StorageStatus::NOT_PRESENT,
                .type = StorageType::UNKNOWN,
                .fs = StorageFs::UNKNOWN,
                .errorCode = 0,
                .errorSymbol = 0,
                .initTime = 0,
                .totalSize = 0,
                .usedSize = 0,
            },
        };
        infoPayload->uptime = millis();
        dispatchMessage(system, createMessage(Command::InfoResponse, now(), infoPayload));
    }

    default:
        break;
    }
    delete message;
}

void DeviceImpl::loop()
{
    BaseDevice::loop();
}

DeviceImpl::DeviceImpl()
{
}

DeviceImpl::~DeviceImpl()
{
}

DeviceImpl Device;