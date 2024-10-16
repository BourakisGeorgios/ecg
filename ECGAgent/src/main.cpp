#include <Arduino.h>
#include "config.h"
#include "sdios.h"
#include "Device.h"
#include "Callback.h"
#include "timer/InaccurateTimer.h"
#include "timer/Timer.h"

#include "modules/ECG.h"
#include <functional>

ECG ecg;

ArduinoOutStream cout(Serial);

void onBpmReceived(double bpm, bool electrodesAttached)
{
  // if (millis() < 15000)
  // {
  //   return;
  // }
  RTCTime time;
  RTC.getTime(time);
  Device.dispatchMessage(createMessage(
      Command::EcgBpm,
      time.getUnixTime(),
      new EcgBpmPayload(bpm)));
  // cout << "BPM: " << bpm << endl;
}
void onElectrodeStatusChanged(bool electrodesAttached)
{
  // if (millis() < 15000)
  // {
  //   return;
  // }
  RTCTime time;
  RTC.getTime(time);
  // Device.dispatchMessage(createMessage(
  //     Command::EcgElectrodeAttached,
  //     time,
  //     new EcgElectrodeStatusPayload(electrodesAttached)));
  // cout << "ELEC: " << electrodesAttached << endl;
}
void onRawMeasurement(int measurement)
{
  // if (millis() < 15000)
  // {
  //   return;
  // }
  // RTCTime time;
  // RTC.getTime(time);
  // Device.dispatchMessage(createMessage(
  //     Command::EcgRaw,
  //     time,
  //     new EcgRawPayload(measurement)));
  // cout << "RAW: " << measurement << endl;
}

void setup()
{
  Serial.begin(9600);
  Device.init();
  ecg.setBpmCallback(onBpmReceived);
  ecg.setElectrodeAttachCallback(onElectrodeStatusChanged);
  ecg.setRawCallback(onRawMeasurement);
  ecg.init();
}

void loop()
{
  Device.loop();
  ecg.loop();
}