#ifndef NULL_STREAM_H
#define NULL_STREAM_H

#include <Print.h>

struct NullStream : public Print
{
    NullStream(void) { return; }
    int available(void) { return 0; }
    void flush(void) { return; }
    int peek(void) { return -1; }
    int read(void) { return -1; }
    size_t write(uint8_t u_Data) { return u_Data, 0x01; }
};

#endif