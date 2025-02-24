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
}

void DeviceImpl::announce()
{
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
        return MessageHandling::MessageContinue;
    }

    dispatchMessage(targetSystem, message, true);

    switch (message->command)
    {
    case Command::EcgBpm:
    {
        auto payload = static_cast<EcgBpmPayload*>(message->payload);
        delete payload;
        break;
    }
    case Command::EcgRaw:
    {
        auto payload = static_cast<EcgRawPayload*>(message->payload);
        delete payload;
        break;
    }
    case Command::EcgElectrodeAttached:
    {
        auto payload = static_cast<EcgElectrodeStatusPayload*>(message->payload);
        delete payload;
        break;
    }
    default:
        break;
    }

    return MessageHandling::MessageHandled;
}

void DeviceImpl::loop()
{
    if (com->getCanTransmit()) {
        while (Serial1.available()) {
            int size = Serial1.available();
            byte *buffer = new byte[size];
            Serial1.readBytes(buffer, size);

            com->getOutPrint()->write(buffer, size);
            delete[] buffer;
        }
    } else {
        BaseDevice::loop();
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