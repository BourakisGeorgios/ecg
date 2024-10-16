#ifndef UTILS_H
#define UTILS_H

#include "Arduino.h"

inline double millisToHz(unsigned long periodMillis) {
    return periodMillis == 0
               ? 0.0
               : 1000.0 / static_cast<double>(periodMillis);
}

#endif
