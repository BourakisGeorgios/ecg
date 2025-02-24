#include "systems/Storage.h"
#include "Device.h"
#include "sdios.h"
#include "StreamUtils.h"
#include <107-Arduino-UniqueId.h>

void fileCreationDateTimeCallback(uint16_t *date, uint16_t *time, uint8_t *ms10)
{
    RTCTime rtcTime;
    RTC.getTime(rtcTime);
    *date = FS_DATE(rtcTime.getYear(), (uint8_t)rtcTime.getMonth(), (uint8_t)rtcTime.getDayOfMonth());
    *time = FS_TIME(rtcTime.getHour(), rtcTime.getMinutes(), rtcTime.getSeconds());
    *ms10 = rtcTime.getSeconds() & 1 ? 100 : 0;
}

bool Storage::create()
{
    char fileName[] = M_DEVICE_STORAGE_DEFAULT_FILE_NAME;
    file.close();
    while (sd.exists(fileName))
    {
        char *p = strrchr(fileName, '.');
        if (!p)
        {
            return false;
        }
        while (true)
        {
            p--;
            if (p < fileName || *p < '0' || *p > '9')
            {
                return -1;
            }
            if (p[0] != '9')
            {
                p[0]++;
                break;
            }
            p[0] = '0';
        }
    }
    if (!file.open(fileName, O_RDWR | O_CREAT))
    {
        return false;
    }

    this->stream = new MessageOutStream(&file);

    if (!writeFileHeader())
    {
        delete this->stream;
        this->stream = nullptr;
        return false;
    }

    return true;
}

bool Storage::writeFileHeader()
{
    if (!file)
    {
        return false;
    }

    RTCTime time;
    RTC.getTime(time);

    auto deviceInfo = Device.getDeviceInfo();
    CommandMessage *m = createMessage(Command::IdentifyResponse, time.getUnixTime(), new IdentifyPayload(deviceInfo), deviceId);
    write(*m);
    delete m;
    return true;
}

void Storage::write(CommandMessage message)
{
    if (!file)
    {
        return;
    }

    this->stream->write(message);
}

uint8_t Storage::init()
{

    FsDateTime::setCallback(fileCreationDateTimeCallback);
    uint32_t t = millis();
    if (!sd.cardBegin(SD_CONFIG))
    {
        return sd.sdErrorCode();
    }

    if (!sd.card()->readCID(&this->cid) || !sd.card()->readCSD(&this->csd) ||
        !sd.card()->readOCR(&this->ocr) || !sd.card()->readSCR(&this->scr))
    {
        return sd.sdErrorCode();
    }

    MbrSector_t mbr;
    if (!sd.card()->readSector(0, reinterpret_cast<uint8_t *>(&mbr)))
    {
        return sd.sdErrorCode();
    }

    if (!sd.volumeBegin())
    {
        return sd.sdErrorCode();
    }

    if (!create())
    {
        return sd.sdErrorCode();
    }

    this->initTime = millis() - t;
    this->status = StorageStatus::OK;
    this->outPrint = this->stream;

    return SD_CARD_ERROR_NONE;
}

void Storage::loop()
{
    if (this->status != StorageStatus::OK)
    {
        return;
    }

    uint32_t t = millis();
    if (this->isInitialized() && t - this->lastSyncTime > 2000)
    {
        this->file.sync();
        this->lastSyncTime = t;
    }
}

StorageType Storage::getStorageType()
{
    switch (sd.card()->type())
    {
    case SD_CARD_TYPE_SD1:
        return StorageType::SD1;
    case SD_CARD_TYPE_SD2:
        return StorageType::SD2;
    case SD_CARD_TYPE_SDHC:
        return csd.capacity() < 70000000
                   ? StorageType::SDHC
                   : StorageType::SDXC;
    default:
        return StorageType::UNKNOWN;
    }
}

StorageFs Storage::getFs()
{
    if (sd.fatType() <= 32)
    {
        switch (sd.fatType())
        {
        case 16:
            return StorageFs::FAT16;
        case 32:
            return StorageFs::FAT32;
        default:
            return StorageFs::UNKNOWN;
        }
    }
    else
    {
        return StorageFs::EXFAT;
    }
}

uint32_t Storage::getTotalSpace()
{
    
    return (uint32_t)floor((0.512 * (double)csd.capacity()));
}

uint32_t Storage::getFreeSpace()
{
    return sd.vol()->freeClusterCount() * sd.vol()->sectorsPerCluster();
}

uint32_t Storage::getUsedSpace()
{
    // return getTotalSpace() - getFreeSpace();
    return getFreeSpace();
}

StorageState Storage::getState()
{
    return {
        this->status,
        this->getStorageType(),
        this->getFs(),
        sd.sdErrorCode(),
        sd.sdErrorData(),
        initTime,
        this->getTotalSpace(),
        this->getUsedSpace()};
}
