#include "timer/InaccurateTimer.h"

void InaccurateTimer::internalLoop() {
    auto currentMillis = ::millis();
    if (currentMillis - this->prevMillis >= this->getMillis())
    {
        this->prevMillis = currentMillis;
        this->getCallback()->invoke();
    }
}