#ifndef TIME_PAYLOAD_H
#define TIME_PAYLOAD_H

#include <Arduino.h>

#include "messages/payloads/IPayload.h"

struct TimePayload : public IPayload
{
    long long data;

    TimePayload(long long data = 0) : data(data) {}

    size_t getSize() override
    {
        return sizeof(TimePayload) - sizeof(IPayload);
    }
} __attribute__((packed));

#endif // TIME_PAYLOAD_H