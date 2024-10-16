#include "Device.h"

#include "SdFat.h"

bool DeviceImpl::initClock()
{
    if (!RTC.begin())
    {
        return 0;
    }
    // SD_CARD_ERROR_CMD0
    RTCTime time;
    RTC.getTime(time);
    RTC.setTime(time);
    return 1;
}

bool DeviceImpl::initStorage()
{
    uint8_t storageInitStatus = storage->init();

    if (storageInitStatus == 0)
    {
        registerSystem(storage);
    }
    return storageInitStatus == 0;
}

bool DeviceImpl::isOk()
{
    return storage->isInitialized() || BaseDevice::isOk();
}

void DeviceImpl::init()
{
    BaseDevice::init();
    initClock();
    initStorage();
}

void DeviceImpl::processMessage(BaseSystem *system, CommandMessage *message)
{
    switch (message->command)
    {
    case Command::IdentifyRequest:
    {
        system->connect(message->targetDeviceId);

        RTCTime time;
        RTC.getTime(time);
        auto response = createMessage(Command::IdentifyResponse, time.getUnixTime(), new IdentifyPayload());
        dispatchMessage(system, response);

        RTC.getTime(time);
        auto timeRequest = createMessage(Command::TimeRequest, time.getUnixTime());
        dispatchMessage(system, timeRequest);
        break;
    }
    case Command::TimeResponse:
    {
        static int i = 0;
        TimePayload *payload = static_cast<TimePayload*>(message->payload);
        RTCTime time(payload->data);
        RTC.setTime(time);
        break;
    }
    case Command::InfoRequest:
    {
        auto *infoPayload = new InfoPayload();
        infoPayload->info = DeviceInfoPayload();
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
            .initialized = storage->isInitialized(),
            .state = storage->getState(),
        };
        infoPayload->uptime = millis();
        RTCTime time;
        RTC.getTime(time);
        dispatchMessage(system, createMessage(Command::InfoResponse, time.getUnixTime(), infoPayload));
    }

    default:
        break;
    }
    delete message;
}

void DeviceImpl::loop()
{
    storage->loop();
    BaseDevice::loop();
}

DeviceImpl::DeviceImpl()
{
    storage = new Storage();
}

DeviceImpl::~DeviceImpl()
{
    delete storage;
}

DeviceImpl Device;