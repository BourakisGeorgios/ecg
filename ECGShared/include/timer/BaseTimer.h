#ifndef BASE_TIMER_H
#define BASE_TIMER_H

#include <Arduino.h>
#include "Callback.h"

class BaseTimer
{
private:
    bool started = false;
    int millis;
    Callback *cb;

protected:
    virtual void internalLoop() {}
    inline Callback *getCallback()
    {
        return cb;
    }

public:
    BaseTimer(int millis, Callback *cb)
    {
        this->millis = millis;
        this->cb = cb;
    }
    ~BaseTimer()
    {
        delete this->cb;
    }
    virtual bool init() { return true; }

    void loop()
    {
        if (isStarted())
        {
            internalLoop();
        }
    }

    virtual bool start()
    {
        return started = true;
    }
    virtual void stop()
    {
        started = false;
    }

    inline bool isStarted()
    {
        return started;
    }

    inline int getMillis()
    {
        return millis;
    }
};

#endif