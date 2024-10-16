#include "Device.h"
#include "UniqueId16.h"
#include "Device.h"

#include "SdFat.h"
#include <UniqueId16.h>

bool DeviceImpl::initClock()
{
    if (!RTC.begin())
    {
        return 0;
    }

    RTCTime time;
    RTC.getTime(time);
    RTC.setTime(time);
    return 1;
}

bool DeviceImpl::isOk()
{
    return BaseDevice::isOk();
}

void DeviceImpl::init()
{
    BaseDevice::init();
    initClock();
}

void DeviceImpl::onComHandshaked()
{
    if (this->com->isConnected())
    {
        if (this->lora->isInitialized())
        {
            this->announceTimer->start();
            // this->announce();
        }
    }
}

void DeviceImpl::announce()
{
    RTCTime time;
    RTC.getTime(time);
    dispatchMessage(
        this->lora,
        createMessage(Command::Discover, time.getUnixTime(), deviceInfo.deviceId));
}

void DeviceImpl::getDeviceId(uint8_t *deviceIdArray)
{
    memcpy(deviceIdArray, deviceInfo.deviceId, 16);
}

MessageHandling DeviceImpl::handleMessage(BaseSystem *system, CommandMessage *message)
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

    dispatchMessage(targetSystem, message);
    return MessageHandling::MessageHandled;

    // switch (message->command)
    // {
    // case Command::IdentifyRequest:
    // {
    //     system->connect(message->targetDeviceId);

    //     RTCTime time;
    //     RTC.getTime(time);
    //     auto response = createMessage(Command::IdentifyResponse, time.getUnixTime(), new IdentifyPayload(deviceId), deviceId);
    //     dispatchMessage(system, response);

    //     RTC.getTime(time);
    //     auto timeRequest = createMessage(Command::TimeRequest, time.getUnixTime(), deviceId);
    //     dispatchMessage(system, timeRequest);
    //     break;
    // }
    // case Command::TimeResponse:
    // {
    //     static int i = 0;
    //     TimePayload *payload = static_cast<TimePayload*>(message->payload);
    //     RTCTime time(payload->data);
    //     RTC.setTime(time);
    //     break;
    // }
    // case Command::InfoRequest:
    // {
    //     auto *infoPayload = new InfoPayload();
    //     infoPayload->info = DeviceInfoPayload(deviceId);
    //     infoPayload->com = COMInfoPayload{
    //         .initialized = com->isInitialized(),
    //         .connected = com->isConnected(),
    //         .isCurrent = system->isType(SystemType::COM),
    //     };
    //     infoPayload->connection = ConnectionInfoPayload{
    //         .initialized = lora->isInitialized(),
    //         .connected = lora->isConnected(),
    //         .isCurrent = system->isType(SystemType::LORA),
    //         .mode = lora->getMode(),
    //     };
    //     infoPayload->storage = StorageInfoPayload{
    //         .initialized = false,
    //         .state = {
    //             .status = StorageStatus::NOT_PRESENT,
    //             .type = StorageType::UNKNOWN,
    //             .fs = StorageFs::UNKNOWN,
    //             .errorCode = 0,
    //             .errorSymbol = 0,
    //             .initTime = 0,
    //             .totalSize = 0,
    //             .usedSize = 0,
    //         },
    //     };
    //     infoPayload->uptime = millis();
    //     RTCTime time;
    //     RTC.getTime(time);
    //     dispatchMessage(system, createMessage(Command::InfoResponse, time.getUnixTime(), infoPayload, deviceId));
    // }

    // default:
    //     break;
    // }
    // delete message;
}

void DeviceImpl::loop()
{
    BaseDevice::loop();
    if (announceTimer->isStarted())
    {
        announceTimer->loop();
    }
}

DeviceInfo constructDeviceInfo()
{

    auto deviceInfo = DeviceInfo{
        .name = M_DEVICE_NAME,
        .type = M_DEVICE_TYPE,
        .version = {M_DEVICE_FIRMWARE_VERSION_MAJOR, M_DEVICE_FIRMWARE_VERSION_MINOR, M_DEVICE_FIRMWARE_VERSION_PATCH},
        .deviceId = {}};
    static auto idInstance = OpenCyphalUniqueId();
    memcpy(deviceInfo.deviceId, idInstance.data(), 16);

    return deviceInfo;
}

DeviceImpl::DeviceImpl() : BaseDevice(constructDeviceInfo(), LoRaBuSMode::Gateway)
{
    this->announceTimer = new InaccurateTimer(10000, new Callback(this, &DeviceImpl::announce));
}

DeviceImpl::~DeviceImpl()
{
}

DeviceImpl Device;