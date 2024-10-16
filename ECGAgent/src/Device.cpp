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

void DeviceImpl::getDeviceId(uint8_t *deviceIdArray)
{
    memcpy(deviceIdArray, deviceInfo.deviceId, 16);
}

MessageHandling DeviceImpl::handleMessage(BaseSystem *system, CommandMessage *message)
{
    RTCTime time;
    RTC.getTime(time);

    switch (message->command)
    {
    case Command::Discover:
    {
        dispatchMessage(this->com, createMessage(Command::Connect, time.getUnixTime(), deviceInfo.deviceId));
        dispatchMessage(system, createMessage(Command::Connect, time.getUnixTime(), deviceInfo.deviceId));
        return MessageHandling::MessageHandled;
        break;
    }
    default:
        break;
    }

    return MessageHandling::MessageContinue;
}

StorageState DeviceImpl::getStorageState()
{
    return storage->getState();
}

void DeviceImpl::loop()
{
    storage->loop();
    BaseDevice::loop();
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

DeviceImpl::DeviceImpl() : BaseDevice(constructDeviceInfo(), LoRaBuSMode::Node)
{
    storage = new Storage();
}

DeviceImpl::~DeviceImpl()
{
    delete storage;
}

DeviceImpl Device;