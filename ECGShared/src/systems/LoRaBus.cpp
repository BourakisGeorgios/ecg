#include "systems/LoRaBus.h"
#include <SPI.h>
#include "BaseDevice.h"

void LoRaBus::broadcastDiscover()
{
}

bool LoRaBus::init()
{
    Serial1.begin(M_DEVICE_COM_SERIAL_BAUDRATE);

    this->initialized = true;
    this->canTransmit = true;
    this->connected = true;

    if (this->initialized)
    {
        this->outPrint = &Serial1;
    }

    return initialized;
}

void LoRaBus::loop()
{
}