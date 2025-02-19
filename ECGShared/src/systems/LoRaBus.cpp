#include "systems/LoRaBus.h"
#include <SPI.h>
#include "BaseDevice.h"

void LoRaBus::broadcastDiscover()
{
    RTCTime time;
    RTC.getTime(time);
    uint8_t deviceId[16] = {0};
    device->getDeviceId(deviceId);

    auto *message = createMessage(Command::Discover, time.getUnixTime(), deviceId);
    this->stream->write(*message);
    delete message;
}

bool LoRaBus::init()
{
    Serial.begin(M_DEVICE_COM_SERIAL_BAUDRATE);

    int remainingAttempts = 20;
    bool gotResponse = false;

    while (!this->initialized && !gotResponse && remainingAttempts != 0)
    {
        remainingAttempts--;
        while (Serial1.available())
        {
            gotResponse = true;
            uint8_t byteRead = Serial1.read();
            this->initialized = byteRead == 0x1;
            remainingAttempts = 0;
            break;
        }

        if (!this->initialized && !gotResponse)
        {
            delay(100);
        }
    }

    if (this->initialized)
    {
        this->inStream = &Serial1;
        this->outPrint = &Serial1;
    }

    return initialized;
}

void LoRaBus::loop()
{
}