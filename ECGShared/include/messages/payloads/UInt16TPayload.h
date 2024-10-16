#ifndef UINT16_T_PAYLOAD_H
#define UINT16_T_PAYLOAD_H

#include <Arduino.h>

#include "messages/payloads/IPayload.h"

struct UInt16TPayload : public IPayload
{
    uint16_t data;

    UInt16TPayload(uint16_t data) : data(data) {}

    size_t getSize() override
    {
        return sizeof(UInt16TPayload);
    }
};

#endif // UINT16_T_PAYLOAD_H