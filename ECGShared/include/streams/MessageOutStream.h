#ifndef MESSAGE_STREAM_H
#define MESSAGE_STREAM_H

#include <Arduino.h>
#include <vector>
#include "messages/messages.h"

class MessageOutStream : public Print
{
private:
    Print *stream;

public:
    MessageOutStream(Print *stream)
    {
        this->stream = stream;
    }

    void flush() override
    {
        stream->flush();
    }

    size_t write(uint8_t data) override
    {
        return stream->write(data);
    }

    size_t write(const uint8_t *buffer, size_t size) override
    {
        return stream->write(buffer, size);
    }

    size_t write(CommandMessage &msg)
    {
        size_t written = 0;
        size_t headerSize = msg.messageSize - msg.payloadSize;
        IPayload *payload = msg.payload;
        msg.payload = 0;
        const uint8_t *headerData = reinterpret_cast<const uint8_t *>(&msg);

        written += stream->write(headerData, headerSize);
        if (payload != nullptr && msg.payloadSize > 0)
        {
            const uint8_t *byteArray = reinterpret_cast<const uint8_t *>(payload) + 4;
            written += stream->write(byteArray, msg.payloadSize);
        }

        return written;
    }
};

#endif