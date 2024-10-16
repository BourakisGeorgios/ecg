#include "BaseDevice.h"

void BaseDevice::registerSystem(BaseSystem *system)
{
    messageForwarder->addSystem(system);
    if (system->getInStream() != nullptr)
    {
        messageBuses.push_back(new MessageBus(this, system));
    }
}

void BaseDevice::dispatchMessage(BaseSystem *system, CommandMessage *message)
{
    MessageForwarder(system).write(*message);
    if (message->payload != nullptr)
    {
        delete message->payload;
    }
    delete message;
}

void BaseDevice::dispatchMessage(CommandMessage *message)
{
    messageForwarder->write(*message);
    if (message->payload != nullptr)
    {
        delete message->payload;
    }
    delete message;
}

BaseDevice::BaseDevice(DeviceInfo info, LoRaBuSMode loraBusMode)
{
    deviceInfo = info;
    messageForwarder = new MessageForwarder();
    com = new COM();
    lora = new LoRaBus(this, loraBusMode);
}

std::vector<MessageBus *> BaseDevice::getMessageBuses()
{
    return std::vector<MessageBus *>(messageBuses);
}

MessageHandling BaseDevice::handleHandshake(BaseSystem *system, CommandMessage *message)
{
    if (this->systemHandshakeMap.find(system) != this->systemHandshakeMap.end())
    {
        this->systemHandshakeMap[system] = false;
    }

    if (this->systemHandshakeMap[system])
    {
        return MessageHandling::MessageContinue;
    }

    uint8_t deviceId[16];
    getDeviceId(deviceId);

    switch (message->command)
    {
    case Command::IdentifyRequest:
    {
        system->connect(message->targetDeviceId);

        RTCTime time;
        RTC.getTime(time);
        auto response = createMessage(Command::IdentifyResponse, time.getUnixTime(), new IdentifyPayload(deviceInfo), deviceId);
        dispatchMessage(system, response);

        RTC.getTime(time);
        auto timeRequest = createMessage(Command::TimeRequest, time.getUnixTime(), deviceId);
        dispatchMessage(system, timeRequest);
        return MessageHandling::MessageHandled;
    }
    case Command::TimeResponse:
    {
        static int i = 0;
        TimePayload *payload = static_cast<TimePayload *>(message->payload);
        RTCTime time(payload->data);
        RTC.setTime(time);
        return MessageHandling::MessageHandled;
    }
    case Command::InfoRequest:
    {
        auto storageState = getStorageState();
        auto *infoPayload = new InfoPayload();
        infoPayload->info = deviceInfo;
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
            .initialized = storageState.status == StorageStatus::OK,
            .state = storageState,
        };
        infoPayload->uptime = millis();
        RTCTime time;
        RTC.getTime(time);
        dispatchMessage(system, createMessage(Command::InfoResponse, time.getUnixTime(), infoPayload, deviceId));
        this->systemHandshakeMap[system] = true;
        onComHandshaked();
        return MessageHandling::MessageHandled;
    }
    default:
        return MessageHandling::MessageContinue;
    }
}

bool BaseDevice::isOk()
{
    return com->isInitialized();
}

void BaseDevice::processMessage(BaseSystem *bus, CommandMessage *message)
{
    if (handleHandshake(bus, message) == MessageHandling::MessageHandled)
    {
        delete message;
        return;
    }

    handleMessage(bus, message);
    delete message;
}

void BaseDevice::init()
{
    initSerial();
    initLoRa();
}

StorageState BaseDevice::getStorageState()
{
    return StorageState{
        .status = StorageStatus::NOT_PRESENT,
        .type = StorageType::UNKNOWN,
        .fs = StorageFs::UNKNOWN,
        .errorCode = 0,
        .errorSymbol = 0,
        .initTime = 0,
        .totalSize = 0,
        .usedSize = 0,
    };
}

void BaseDevice::loop()
{
    for (auto messageBus : messageBuses)
    {
        messageBus->getSystem()->loop();
        messageBus->loop();
    }
    lora->loop();
}

bool BaseDevice::initSerial()
{
    auto comInit = com->init();

    if (comInit)
    {
        registerSystem(com);
    }

    return comInit;
}

bool BaseDevice::initLoRa()
{
    auto loraInit = lora->init();

    if (loraInit)
    {
        registerSystem(lora);
    }

    return loraInit;
}