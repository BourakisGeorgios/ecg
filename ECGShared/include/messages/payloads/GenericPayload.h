#ifndef GENERIC_PAYLOAD_H
#define GENERIC_PAYLOAD_H

#include <Arduino.h>

template <typename T>
struct GenericPayload : public IPayload
{
    size_t size;
    T data;

    GenericPayload(T data, size_t size) : data(data), size(size) {}

    size_t getSize() override
    {
        return size;
    }
};

#endif // GENERIC_PAYLOAD_H