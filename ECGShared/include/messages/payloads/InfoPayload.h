#ifndef INFO_PAYLOAD_H
#define INFO_PAYLOAD_H

#include <Arduino.h>

#include "messages/payloads/IPayload.h"
#include "messages/DeviceType.h"
#include "messages/DeviceInfo.h"
#include "types/StorageTypes.h"
#include "types/LoRaTypes.h"

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
    DeviceInfo info;
    StorageInfoPayload storage;
    ConnectionInfoPayload connection;
    COMInfoPayload com;

    size_t getSize() override
    {
        return sizeof(InfoPayload) - sizeof(IPayload);
    }
} __attribute__((packed));

#endif // INFO_PAYLOAD_H