#ifndef IDENTIFY_PAYLOAD_H
#define IDENTIFY_PAYLOAD_H

#include "sharedConfig.h"
#include "messages/payloads/IPayload.h"
#include "messages/payloads/InfoPayload.h"

struct IdentifyPayload : public DeviceInfoPayload, public IPayload
{
    IdentifyPayload() : DeviceInfoPayload() {}

    size_t getSize() override
    {
        return sizeof(IdentifyPayload) - sizeof(IPayload);
    }
} __attribute__((packed));

#endif // IDENTIFY_PAYLOAD_H