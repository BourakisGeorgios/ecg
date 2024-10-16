#ifndef UTILS_H
#define UTILS_H

#include "Arduino.h"

inline float millisToHz(unsigned long periodMillis)
{
    return periodMillis == 0
               ? 0
               : 1000.0 / periodMillis;
}

#endif