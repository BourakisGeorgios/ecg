#include "timer/Timer.h"

#ifndef ARDUINO_ARCH_AVR
 
bool Timer::start()
{
    uint8_t timer_type = GPT_TIMER;
    int8_t tindex = FspTimer::get_available_timer(timer_type);
    if (tindex < 0)
    {
        tindex = FspTimer::get_available_timer(timer_type, true);
    }
    if (tindex < 0)
    {
        return false;
    }

    FspTimer::force_use_of_pwm_reserved_timer();

    if (!timer.begin(
            TIMER_MODE_PERIODIC,
            timer_type,
            tindex,
            millisToHz(getMillis()),
            0.0f,
            [](timer_callback_args_t *p_args)
            {
                Callback *cb = (Callback *)(p_args->p_context);
                cb->invoke();
            },
            (void *)this->getCallback()))
    {
        return false;
    }

    if (!timer.setup_overflow_irq())
    {
        return false;
    }

    if (!timer.open())
    {
        return false;
    }

    if (!timer.start())
    {
        return false;
    }
    return BaseTimer::start();
}

void Timer::stop()
{
    timer.stop();
    timer.close();

    BaseTimer::stop();
}

#endif