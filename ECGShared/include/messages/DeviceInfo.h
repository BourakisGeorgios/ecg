#ifndef DEVICE_INFO_H
#define DEVICE_INFO_H

#include <Arduino.h>
#include "sharedConfig.h"
#include "messages/DeviceType.h"

struct DeviceInfo {
    char name[5] = M_DEVICE_NAME;
    DeviceType type = DeviceType::Unknown;
    uint8_t version[3] = {};
    uint8_t deviceId[16] = {0};
}  __attribute__((packed));

#endif