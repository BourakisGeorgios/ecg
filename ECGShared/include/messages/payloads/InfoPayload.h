#ifndef INFO_PAYLOAD_H
#define INFO_PAYLOAD_H

#include <Arduino.h>

#include "messages/payloads/IPayload.h"
#include "messages/DeviceType.h"
#include "types/StorageTypes.h"
#include "types/LoRaTypes.h"
#include "polyfills/UniqueId.h"

struct DeviceInfoPayload
{
public:
    char name[5];
    DeviceType type;
    uint8_t version[3];
    uint8_t deviceId[16];

    DeviceInfoPayload() : type(M_DEVICE_TYPE)
    {
        const uint8_t version[3] = {M_DEVICE_FIRMWARE_VERSION_MAJOR, M_DEVICE_FIRMWARE_VERSION_MINOR, M_DEVICE_FIRMWARE_VERSION_PATCH};
        strncpy(this->name, M_DEVICE_NAME, sizeof(M_DEVICE_NAME) - 1);
        this->name[sizeof(this->name) - 1] = '\0';
        memcpy(this->version, version, sizeof(this->version));
        getDeviceId(this->deviceId);
        // memcpy(this->deviceId, OpenCyphalUniqueId().data(), sizeof(this->deviceId));
    }
} __attribute__((packed));

struct StorageInfoPayload
{
    bool initialized;
    StorageState state;
} __attribute__((packed));

struct ConnectionInfoPayload
{
    bool initialized;
    bool connected;
    bool isCurrent;
    LoRaBuSMode mode;
} __attribute__((packed));

struct COMInfoPayload
{
    bool initialized;
    bool connected;
    bool isCurrent;
} __attribute__((packed));

struct InfoPayload : public IPayload
{
    uint64_t uptime;
    DeviceInfoPayload info;
    StorageInfoPayload storage;
    ConnectionInfoPayload connection;
    COMInfoPayload com;

    size_t getSize() override
    {
        return sizeof(InfoPayload) - sizeof(IPayload);
    }
} __attribute__((packed));

#endif // INFO_PAYLOAD_H