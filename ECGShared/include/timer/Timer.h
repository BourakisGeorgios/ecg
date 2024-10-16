#ifndef CUSTOM_TIMER_H
#define CUSTOM_TIMER_H

#ifndef ARDUINO_ARCH_AVR

#include "timer/BaseTimer.h"
#include "FspTimer.h"
#include "utils.h"

class Timer : public BaseTimer
{
private:
    FspTimer timer;
public:
    // template <class T>
    // Timer(int millis, T *obj, TIMER_MEMBER_CB<T> timerCallback) : BaseTimer(millis,obj,timerCallback)
    // {
    // }
    // Timer(int millis, TIMER_GLOBAL_CB timerCallback) : BaseTimer(millis, timerCallback)
    // {
    // }
    Timer(int millis, Callback *callback) : BaseTimer(millis, callback)
    {
    }
    bool start() override;
    void stop() override;
};

#endif

#endif // CUSTOM_TIMER_H