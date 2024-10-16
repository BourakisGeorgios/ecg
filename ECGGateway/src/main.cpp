#include <Arduino.h>
#include "sharedConfig.h"
#include "Device.h"

void setup()
{
    Device.init();
}

void loop()
{
    Device.loop();
}