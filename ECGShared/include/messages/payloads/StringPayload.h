#ifndef STRING_PAYLOAD_H
#define STRING_PAYLOAD_H

#include "messages/payloads/IPayload.h"

struct StringPayload : public IPayload
{
    char data[33];

    StringPayload(const char *data)
    {
        strncpy(this->data, data, 32);
        this->data[32] = '\0';
    }

    StringPayload(const String &str)
    {
        strncpy(this->data, str.c_str(), 32);
        this->data[32] = '\0';
    }

    operator String() const
    {
        return String(this->data);
    }

    size_t getSize() override
    {
        return sizeof(StringPayload);
    }
};
#endif // STRING_PAYLOAD_H