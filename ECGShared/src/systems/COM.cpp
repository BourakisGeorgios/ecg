#include "systems/COM.h"
#include "sharedConfig.h"

bool COM::init()
{
    Serial.begin(M_DEVICE_COM_SERIAL_BAUDRATE);
    this->initialized = Serial;
    
    if (this->initialized)
    {
        this->inStream = &Serial;
        this->outPrint = &Serial;
    }

    return this->initialized;
}