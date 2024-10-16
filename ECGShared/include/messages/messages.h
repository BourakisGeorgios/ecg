#ifndef MESSAGES_H
#define MESSAGES_H

#include <Arduino.h>
#include <sdios.h>

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
    Discover = 0xC,
    Connect = 0xD,
    Disconnect = 0xE,
    Ping = 0xF,
    Pong = 0x10,

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

inline CommandMessage *createMessage(Command command, uint32_t millis, long long time, IPayload *payload, uint8_t deviceId[16] = {0})
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
    memcpy(&message->deviceId, deviceId + 10, 6);
    return message;
}

inline CommandMessage *createMessage(Command command, long long time, IPayload *payload, uint8_t deviceId[16] = {0})
{
    return createMessage(command, millis(), time, payload, deviceId);
}

template <typename T>
inline CommandMessage *createMessage(Command command, long long time, T payload, size_t payloadSize, uint8_t deviceId[16] = {0})
{
    IPayload *messagePayload = new GenericPayload<T>(payload, payloadSize);

    return createMessage(
        command,
        time,
        messagePayload,
        deviceId);
}

inline CommandMessage *createMessage(Command command, long long time, String str, uint8_t deviceId[16] = {0})
{
    size_t strSize = str.length() + 1;
    byte buffer[strSize];
    str.getBytes(buffer, strSize);

    return createMessage(
        command,
        time,
        new StringPayload(str),
        deviceId);
}

inline CommandMessage *createMessage(Command command, long long time, uint8_t deviceId[16] = {0})
{
    return createMessage(
        command,
        time,
        (IPayload *)nullptr,
        deviceId);
}

#endif