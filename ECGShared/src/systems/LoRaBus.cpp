#include "systems/LoRaBus.h"
#include <SPI.h>
bool LoRaBus::init()
{
    LoRa.setPins(M_DEVICE_LORA_NSS_PIN, M_DEVICE_LORA_NRESET_PIN, M_DEVICE_LORA_DIO0_PIN);
    initialized = LoRa.begin(M_DEVICE_LORA_DEFAULT_FREQ);

    if (initialized)
    {
        this->inStream = this->stream;
        this->outPrint = this->stream;
    }

    return initialized;
}