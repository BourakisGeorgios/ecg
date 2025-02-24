#include "messaging/MessageBus.h"
#include "sharedConfig.h"
#include "BaseDevice.h"


template <typename T>
bool MessageBus::readBytes(T *dest, size_t size)
{
    if (this->remainingBytes < size)
    {
        return false;
    }

    memcpy(dest, this->messageBuffer + midx, size);
    midx += size;
    this->remainingBytes -= size;
    return true;
}

bool MessageBus::readSingle(byte *byte)
{
    if (this->remainingBytes == 0)
    {
        return false;
    }

    *byte = this->messageBuffer[this->midx];
    this->midx++;
    this->remainingBytes--;
    return true;
}

CommandMessage *MessageBus::recv()
{
    auto stream = this->system->getInStream();
    while (stream->available() > 0 || this->remainingBytes > 0)
    {
        if (this->messageBuffer == nullptr)
        {
            this->bufferLength = stream->available();
            this->messageBuffer = new byte[bufferLength];
            this->remainingBytes = stream->readBytes(this->messageBuffer, bufferLength);
        }
        else if (stream->available() > 0)
        {
            int bytesAvailable = stream->available();
            byte *newBuffer = new byte[bytesAvailable];
            int newBytesRead = stream->readBytes(newBuffer, bytesAvailable);
            byte *replacementBuffer;
            int startingIndex = 0;
            if (this->remainingBytes > 0)
            {
                startingIndex = bufferLength;
                replacementBuffer = new byte[this->bufferLength + newBytesRead];
                memcpy(replacementBuffer, this->messageBuffer, bufferLength);
            }
            else
            {
                replacementBuffer = new byte[newBytesRead];
                startingIndex = 0;
                remainingBytes = 0;
                midx = 0;
            }
            memcpy(replacementBuffer + startingIndex, newBuffer, newBytesRead);
            delete[] this->messageBuffer;
            delete[] newBuffer;
            this->messageBuffer = replacementBuffer;
            this->remainingBytes += newBytesRead;
            this->bufferLength = startingIndex + newBytesRead;
            
        }


        while (this->remainingBytes > 0 && !this->commandMessageReceived)
        {
            if (!this->recvInProgress)
            {
                clear(false);
                this->recvInProgress = true;
            }

            if (!this->readHeaders())
            {
                break;
            }
        }

        if (!this->recvInProgress && this->commandMessageReceived)
        {
            return parseCommand();
        }
    }

    return nullptr;
}

CommandMessage *MessageBus::parseCommand()
{
    if (this->commandMessageReceived == false)
        return nullptr;

    this->commandMessageReceived = false;

    if (hasCommandReceived == false)
        return nullptr;

    auto constructMessage = [this](IPayload *payload = nullptr)
    {
        return createMessage(this->commandReceived, millis(), this->rtcTimeReceived, payload);
    };

    CommandMessage *receivingMessage = nullptr;

    switch (this->commandReceived)
    {
    case Command::IdentifyRequest:
    {
        receivingMessage = constructMessage();
        break;
    }
    case Command::TimeResponse:
    {
        auto time = reinterpret_cast<long long *>(this->payloadBuffer);
        TimePayload *timePayload = new TimePayload();
        timePayload->data = *time;
        receivingMessage = constructMessage(timePayload);
        break;
    }
    case Command::InfoRequest:
    {
        receivingMessage = constructMessage();
        break;
    }
    case Command::EcgBpm:
    {
        auto bpm = reinterpret_cast<double *>(this->payloadBuffer);
        EcgBpmPayload *bpmPayload = new EcgBpmPayload(*bpm);
        receivingMessage = constructMessage(bpmPayload);
        break;
    }
    default:
        break;
    }

    if (this->payloadBuffer != nullptr)
    {
        delete[] this->payloadBuffer;
        this->payloadBuffer = nullptr;
    }
    return receivingMessage;
}

void MessageBus::clear(bool fatal)
{
    if (fatal)
    {
        this->completeConsume();
    }
    else
    {
        midx = 0;
    }
    recvInProgress = false;
    commandMessageReceived = false;
    hasCommandReceived = false;
    hasOriginDeviceIdReceived = false;
    hasTargetDeviceIdReceived = false;
    hasSizeReceived = false;
    hasRtcTimeReceived = false;
    hasPayloadSizeReceived = false;
    hasTimeReceived = false;
    payloadSizeReceived = 0;
    commandReceived = Command::Unkn;
    memset(originDeviceIdReceived, 0, 6);
    memset(targetDeviceIdReceived, 0, 6);
    sizeReceived = 0;
    timeReceived = 0;
    rtcTimeReceived = 0;
    if (payloadBuffer != nullptr)
    {
        delete[] payloadBuffer;
        payloadBuffer = nullptr;
    }
}

bool MessageBus::readHeaders()
{
    if (!hasCommandReceived)
    {
        byte commandByte = 0x0;
        if (!this->readSingle(&commandByte))
        {
            return false;
        }

        if (commandByte > (byte)Command::Total)
        {
            clear();
            return false;
        }
        
        commandReceived = (Command)commandByte;
        hasCommandReceived = true;
    }

    else if (!hasSizeReceived)
    {
        if (!readBytes(&this->sizeReceived, sizeof(size_t)))
        {
            return false;
        }

        if (this->sizeReceived < 0)
        {
            clear();
            return false;
        }

        hasSizeReceived = true;
    }

    else if (!hasOriginDeviceIdReceived)
    {
        if (!readBytes(&this->originDeviceIdReceived, 6))
        {
            return false;
        }
        
        hasOriginDeviceIdReceived = true;
    }
    else if (!hasTargetDeviceIdReceived)
    {
        if (!readBytes(&this->targetDeviceIdReceived, 6))
        {
            return false;
        }

        
        hasTargetDeviceIdReceived = true;
    }
    else if (!hasTimeReceived)
    {
        if (!readBytes(&this->timeReceived, sizeof(uint32_t)))
        {
            return false;
        }
        
        hasTimeReceived = true;
    }

    else if (!hasRtcTimeReceived)
    {
        if (!readBytes(&this->rtcTimeReceived, sizeof(long long)))
        {
            return false;
        }

        
        hasRtcTimeReceived = true;
    }
    else if (!hasPayloadSizeReceived)
    {
        if (!readBytes(&this->payloadSizeReceived, sizeof(size_t)))
        {
            return false;
        }

        if (payloadSizeReceived < 0)
        {
            clear();
            return false;
        }
        hasPayloadSizeReceived = true;
        if (this->payloadSizeReceived == 0)
        {
            this->completeConsume();
            return true;
        }
        else
        {
            this->payloadBuffer = new byte[this->payloadSizeReceived];
        }
    }

    else if (this->hasPayloadSizeReceived)
    {

        if (!readBytes(this->payloadBuffer, this->payloadSizeReceived))
        {
            return false;
        }
        else
        {
            this->completeConsume();
            return true;
        }
    }

    return true;
}

void MessageBus::completeConsume()
{
    this->recvInProgress = false;
    this->commandMessageReceived = true;
    if (this->remainingBytes > 0)
    {
        
        byte *newBuffer = new byte[this->remainingBytes];
        memcpy(newBuffer, this->messageBuffer + this->midx, this->bufferLength - this->midx);
        this->remainingBytes = this->bufferLength - this->midx;
        this->bufferLength = this->remainingBytes;
        delete[] this->messageBuffer;
        this->messageBuffer = newBuffer;
        
    }
    else
    {
        delete[] this->messageBuffer;
        this->messageBuffer = nullptr;
        this->bufferLength = 0;
        this->remainingBytes = 0;
    }

    this->midx = 0;
}

void MessageBus::loop()
{
    CommandMessage *message = this->recv();
    if (message != nullptr)
    {
        clear(false);
        this->device->processMessage(this->system, message);
    }
}