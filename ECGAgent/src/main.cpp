#include <Arduino.h>
#include "config.h"
#include "Device.h"
#include <memory>

#include <SPI.h>
#include <LoRa.h>

#include "messages/messages.h"
#include "modules/BaseECG.h"

#ifdef M_DEVICE_FAKE_ECG
#include "modules/FakeECG.h"
BaseECG *ecg = new FakeECG();
#else
#include "modules/ECG.h"
BaseECG *ecg = new ECG();
#endif

void onBpmReceived(double bpm, bool electrodesAttached)
{
  RTCTime time;
  RTC.getTime(time);
  auto payload = new EcgBpmPayload(bpm);
  Device.dispatchMessage(createMessage(
      Command::EcgBpm,
      time.getUnixTime(),
      payload));

  delete payload;
}
void onElectrodeStatusChanged(bool electrodesAttached)
{
  RTCTime time;
  RTC.getTime(time);
  auto payload = new EcgElectrodeStatusPayload(electrodesAttached);
  Device.dispatchMessage(createMessage(
      Command::EcgElectrodeAttached,
      time.getUnixTime(),
      payload));

  delete payload;
}
void onRawMeasurement(int measurement)
{
  RTCTime time;
  RTC.getTime(time);

  auto payload = new EcgRawPayload(measurement);
  Device.dispatchMessage(createMessage(
      Command::EcgRaw,
      time.getUnixTime(),
      payload));

  delete payload;
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