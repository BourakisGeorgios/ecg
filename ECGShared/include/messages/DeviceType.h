#ifndef DEVICE_TYPE_H
#define DEVICE_TYPE_H

#include <Arduino.h>

enum class DeviceType : byte
{
    Unknown = 0x0,
    Node = 0x1,
    Controller = 0x2,
    Gateway = 0x3,
};

#endif