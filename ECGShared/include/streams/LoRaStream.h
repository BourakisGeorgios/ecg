#ifndef LORA_STREAM_H
#define LORA_STREAM_H

#include <Arduino.h>
#include <LoRa.h>
#include "systems/BaseSystem.h"
#include "streams/MessageOutStream.h"

class LoRaStream : public Stream
{
private:
    LoRaClass *lora;
    MessageOutStream *forwarder;

public:
    LoRaStream(LoRaClass *lora)
    {
        this->lora = lora;
        this->forwarder = new MessageOutStream(lora);
    }
    ~LoRaStream()
    {
        delete forwarder;
    }

    int available() override {
        int available = lora->parsePacket();
        if (!available) {
            return 0;
        }

        return available;
    }

    int read() override {
        return lora->read();
    }

    int peek() override {
        return lora->peek();
    }

    void flush() override
    {
        forwarder->flush();
    }

    size_t write(uint8_t data) override
    {
        return forwarder->write(data);
    }

    size_t write(const uint8_t *buffer, size_t size) override
    {
        return forwarder->write(buffer, size);
    }

    size_t write(CommandMessage &msg)
    {
        lora->beginPacket();
        auto size = forwarder->write(msg);
        lora->endPacket();
        return size;
    }
};

#endif // LORA_STREAM_H