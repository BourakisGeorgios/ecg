#ifndef ECG_ELECTRODE_STATUS_PAYLOAD_H
#define ECG_ELECTRODE_STATUS_PAYLOAD_H

#include <Arduino.h>

#include "messages/payloads/IPayload.h"

struct EcgElectrodeStatusPayload : public IPayload
{
    bool data;

    EcgElectrodeStatusPayload(bool data) : data(data) {}

    size_t getSize() override
    {
        return sizeof(EcgElectrodeStatusPayload) - sizeof(IPayload);
    }
}  __attribute__((packed));

#endif // ECG_ELECTRODE_STATUS_PAYLOAD_H