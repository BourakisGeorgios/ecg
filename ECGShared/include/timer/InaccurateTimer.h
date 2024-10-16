#ifndef CUSTOM_INACCURATE_TIMER_H
#define CUSTOM_INACCURATE_TIMER_H

#include "timer/BaseTimer.h"


class InaccurateTimer : public BaseTimer
{
private:
    int prevMillis;
protected:
    void internalLoop() override;
public:
    // template <class T>
    // InaccurateTimer(int millis, T *obj, TIMER_MEMBER_CB<T> timerCallback) : BaseTimer(millis,obj,timerCallback)
    // {
    // }
    // InaccurateTimer(int millis, TIMER_GLOBAL_CB timerCallback) : BaseTimer(millis, timerCallback)
    // {
    // }    
    InaccurateTimer(int millis, Callback* cb) : BaseTimer(millis, cb)
    {
    }
};

#endif // CUSTOM_TIMER_H