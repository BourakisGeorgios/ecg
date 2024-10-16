#ifndef LORA_TYPES_H
#define LORA_TYPES_H

#include <Arduino.h>

enum class LoRaBuSMode : byte
{
    Gateway = 0x0,
    Node = 0x1
};

#endif