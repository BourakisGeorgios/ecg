#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>
#include <RTC.h>

#include "config.h"
#include "types/StorageTypes.h"
#include "messages/messages.h"

#include "systems/BaseSystem.h"
#include "streams/MessageForwarder.h"
#include "streams/MessageOutStream.h"

#define SD_CONFIG SdSpiConfig(M_DEVICE_STORAGE_SD_CARD_PIN, SHARED_SPI, SD_SCK_MHZ(16))

const uint32_t PREALLOCATE_SIZE_MiB = 1UL;
const uint64_t PREALLOCATE_SIZE = (uint64_t)PREALLOCATE_SIZE_MiB << 20;

class Storage : public BaseSystem
{
private:
    SdFs sd;
    cid_t cid;
    csd_t csd;
    scr_t scr;
    uint32_t ocr;
    uint32_t initTime;
    FsFile file;
    time_t lastSyncTime = 0;
    StorageStatus status = StorageStatus::NOT_OK;
    MessageOutStream* stream;
    bool create();
    bool writeFileHeader();

public:
    Storage()
    {
        this->supportedInCommands
            .assign({(byte)Command::IdentifyRequest,
                     (byte)Command::InfoRequest,
                     (byte)Command::TimeResponse});
        this->supportedOutCommands
            .assign({(byte)Command::IdentifyRequest,
                     (byte)Command::IdentifyResponse,
                     (byte)Command::InfoRequest,
                     (byte)Command::InfoResponse,
                     (byte)Command::TimeRequest,
                     (byte)Command::TimeResponse,
                     (byte)Command::EcgBpm,
                     (byte)Command::EcgElectrodeAttached,
                    //  (byte)Command::EcgRaw,
                     (byte)Command::Log,
                     (byte)Command::Debug});
    }
    bool isInitialized() override { return status == StorageStatus::OK; }
    uint8_t init();
    StorageState getState();
    void write(CommandMessage message);
    void loop();
    StorageType getType();
    StorageFs getFs();
    uint32_t getTotalSpace();
    uint32_t getUsedSpace();
    uint32_t getFreeSpace();

    bool isType(SystemType type) override
    {
        return type == SystemType::STORAGE;
    }
};

#endif