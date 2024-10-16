#ifndef C_UNIQUE_ID_H
#define C_UNIQUE_ID_H




#include <Arduino.h>
#ifdef ARDUINO_ARCH_AVR
inline void getDeviceId(uint8_t*array) {
    memset(array, 0, 16);
}
#else
#include <UniqueId16.h>
inline void getDeviceId(uint8_t*array) {
    static auto idInstance = OpenCyphalUniqueId();
    memcpy(array, idInstance.data(), 16);
}
#endif
#endif