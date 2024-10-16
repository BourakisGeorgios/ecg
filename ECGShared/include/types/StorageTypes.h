#ifndef STORAGE_TYPES_H
#define STORAGE_TYPES_H

#include <Arduino.h>

enum class StorageStatus : byte
{
    OK = 0x1,
    NOT_OK = 0x0,
    NOT_PRESENT = 0x2,
};

enum class StorageType : byte
{
    UNKNOWN = 0x0,
    SD1 = 0x1,
    SD2 = 0x2,
    SDHC = 0x3,
    SDXC = 0x4
};

enum class StorageFs : byte
{
    UNKNOWN = 0x0,
    FAT16 = 0x1,
    FAT32 = 0x2,
    EXFAT = 0x3
};

struct StorageState
{
    StorageStatus status;
    StorageType type;
    StorageFs fs;
    uint8_t errorCode;
    uint8_t errorSymbol;
    uint32_t initTime;
    uint32_t totalSize;
    uint32_t usedSize;
} __attribute__((packed));

#endif