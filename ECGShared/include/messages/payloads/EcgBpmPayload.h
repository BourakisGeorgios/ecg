#ifndef ECG_BPM_PAYLOAD_H
#define ECG_BPM_PAYLOAD_H

#include <Arduino.h>

#include "messages/payloads/IPayload.h"

struct EcgBpmPayload : public IPayload
{
    double bpm;

    EcgBpmPayload(double bpm) : bpm(bpm) {}

    size_t getSize() override
    {
        return sizeof(EcgBpmPayload) - sizeof(IPayload);
    }
}  __attribute__((packed));

#endif // ECG_BPM_PAYLOAD_H