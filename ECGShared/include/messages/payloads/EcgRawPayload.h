#ifndef ECG_RAW_PAYLOAD_H
#define ECG_RAW_PAYLOAD_H

#include <Arduino.h>

#include "messages/payloads/IPayload.h"

struct EcgRawPayload : public IPayload
{
    int raw;

    EcgRawPayload(int raw) : raw(raw) {}

    size_t getSize() override
    {
        return sizeof(EcgRawPayload) - sizeof(IPayload);
    }
}  __attribute__((packed));

#endif // ECG_RAW_PAYLOAD_H