#include <Arduino.h>
#include "config.h"
#include "Device.h"

#include <SPI.h>
#include <LoRa.h>

#include "messages/messages.h"
#include "modules/BaseECG.h"

#ifdef M_DEVICE_FAKE_ECG
#include "modules/FakeECG.h"
BaseECG* ecg = new FakeECG();
#else
#include "modules/ECG.h"
BaseECG* ecg = new ECG();
#endif

void onBpmReceived(double bpm, bool electrodesAttached)
{
  if (millis() < 15000)
  {
    return;
  }
  RTCTime time;
  RTC.getTime(time);
  Device.dispatchMessage(createMessage(
      Command::EcgBpm,
      time.getUnixTime(),
      new EcgBpmPayload(bpm)));
}
void onElectrodeStatusChanged(bool electrodesAttached)
{
  if (millis() < 15000)
  {
    return;
  }
  RTCTime time;
  RTC.getTime(time);
  Device.dispatchMessage(createMessage(
      Command::EcgElectrodeAttached,
      time.getUnixTime(),
      new EcgElectrodeStatusPayload(electrodesAttached)));
}
void onRawMeasurement(int measurement)
{
  if (millis() < 15000)
  {
    return;
  }
  RTCTime time;
  RTC.getTime(time);
  Device.dispatchMessage(createMessage(
      Command::EcgRaw,
      time.getUnixTime(),
      new EcgRawPayload(measurement)));
}

void setup()
{
  Device.init();
  ecg->setBpmCallback(onBpmReceived);
  ecg->setElectrodeAttachCallback(onElectrodeStatusChanged);
  ecg->setRawCallback(onRawMeasurement);
  ecg->init();
}

void loop()
{
  Device.loop();
  ecg->loop();
}