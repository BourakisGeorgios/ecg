#ifndef I_PAYLOAD_H
#define I_PAYLOAD_H

#include <Arduino.h>

struct IPayload
{
    virtual size_t getSize();
} __attribute__((packed));

#endif // I_PAYLOAD_H