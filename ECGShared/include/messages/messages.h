#ifndef MESSAGES_H
#define MESSAGES_H

#include <Arduino.h>
#include <sdios.h>

#include "polyfills/UniqueId.h"
#include "messages/payloads/payloads.h"

enum class Command : byte
{
    Unkn = 0x0,
    IdentifyRequest = 0x1,
    IdentifyResponse = 0x2,
    TimeRequest = 0x3,
    TimeResponse = 0x4,
    InfoRequest = 0x5,
    InfoResponse = 0x6,
    Debug = 0x7,
    EcgRaw = 0x8,
    EcgElectrodeAttached = 0x9,
    EcgBpm = 0xA,
    Log = 0xB,
    Connect = 0xC,
    Ping = 0xD,
    Pong = 0xE,

    Total
};

struct CommandMessage
{
    Command command;
    size_t messageSize;
    uint8_t deviceId[6];
    uint8_t targetDeviceId[6];
    uint32_t time;
    long long rtcTime;
    size_t payloadSize;
    IPayload *payload;
} __attribute__((packed));

inline CommandMessage *createMessage(Command command, uint32_t millis, long long time, IPayload *payload)
{
    size_t payloadSize = payload != nullptr ? payload->getSize() : 0;
    size_t messageSize = sizeof(CommandMessage) - sizeof(IPayload *) + payloadSize;
    auto message = new CommandMessage{
        .command = command,
        .messageSize = messageSize,
        .deviceId = {},
        .targetDeviceId = {},
        .time = millis,
        .rtcTime = time,
        .payloadSize = payloadSize,
        .payload = payloadSize > 0 ? payload : nullptr,
    };
    uint8_t deviceId[16] = {0};
    getDeviceId(deviceId);
    memcpy(&message->deviceId, deviceId + 10, 6);
    return message;
}

inline CommandMessage *createMessage(Command command, long long time, IPayload *payload)
{
    return createMessage(command, millis(), time, payload);
}

template <typename T>
inline CommandMessage *createMessage(Command command, long long time, T payload, size_t payloadSize)
{
    IPayload *messagePayload = new GenericPayload<T>(payload, payloadSize);

    return createMessage(
        command,
        time,
        messagePayload);
}

inline CommandMessage *createMessage(Command command, long long time, String str)
{
    size_t strSize = str.length() + 1;
    byte buffer[strSize];
    str.getBytes(buffer, strSize);

    return createMessage(
        command,
        time,
        new StringPayload(str));
}

inline CommandMessage *createMessage(Command command, long long time)
{
    return createMessage(
        command,
        time,
        (IPayload *)nullptr);
}

#endif