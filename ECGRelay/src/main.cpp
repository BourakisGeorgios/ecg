#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>
#include <sdios.h>
#include <vector>

#define M_DEVICE_LORA_DEFAULT_FREQ 868E6
#define M_DEVICE_LORA_NSS_PIN 8
#define M_DEVICE_LORA_NRESET_PIN 9
#define M_DEVICE_LORA_DIO0_PIN 2
#define M_DEVICE_LORA_BUFFER_LOOP_INTERVAL_MS 900

#if defined(M_DEVICE_LORA_GW) && defined(M_DEVICE_LORA_NODE)
#error The device should act as a gateway or node, not both.
#elif !defined(M_DEVICE_LORA_GW) && !defined(M_DEVICE_LORA_NODE)
#error The device should act as a gateway or node, none defined. \
       Conider defining either M_DEVICE_LORA_NODE or M_DEVICE_LORA_GW
#endif

ArduinoOutStream cout(Serial);

#define DEBUG(x) \
  do             \
  {              \
    if (Serial)  \
    {            \
      cout << x; \
    }            \
  } while (0)

bool initialized = false;
bool reportedStatus = false;
ulong batch = 0;
std::vector<byte> bufferVec = {};

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);

  delay(1000);

  if (Serial)
  {
    cout << "Initializing LoRa Relay." << endl;
  }

  LoRa.setPins(M_DEVICE_LORA_NSS_PIN, M_DEVICE_LORA_NRESET_PIN, M_DEVICE_LORA_DIO0_PIN);
  initialized = LoRa.begin(M_DEVICE_LORA_DEFAULT_FREQ) == 1;

  if (!initialized)
  {
    while (true)
    {

      Serial1.write((byte)(initialized ? 0x1 : 0x0));
      delay(500);
      DEBUG("LoRa Antenna failed to initialize. " << endl);
    }
  }

  // Serial1.flush();
  LoRa.setSyncWord(0x21);
  DEBUG("LoRa Antenna initialized. " << endl);

#ifdef M_DEVICE_LORA_GW
  LoRa.receive();
  DEBUG("LoRa Antenna set to receive only mode. " << endl);
#endif
}

int prevMillis = 0;
int mills = 0;
bool loopEvery(int ms) {
    auto currentMillis = millis();
    if (currentMillis - prevMillis >= ms)
    {
        prevMillis = currentMillis;
        return true;
    } else {
      return false;
    }
}

#ifdef M_DEVICE_LORA_NODE
void loop_node()
{
  int received = Serial1.available();
  if (received > 0)
  {
    while (Serial1.available())
    {
      bufferVec.push_back(Serial1.read());
    }
  }
  if (loopEvery(M_DEVICE_LORA_BUFFER_LOOP_INTERVAL_MS) && bufferVec.size() > 0) {
    Serial1.write(bufferVec.data(), bufferVec.size());
    LoRa.beginPacket();
    LoRa.write(bufferVec.data(), bufferVec.size());

    if (!LoRa.endPacket(false))
    {
      DEBUG("Error Forwarding: " << bufferVec.size() << " bytes from COM to LoRa" << endl);
    }
    else
    {
      DEBUG("Forwarded: " << bufferVec.size() << " bytes from COM to LoRa" << endl);
    }
    bufferVec.clear();
  }
}
#endif

#ifdef M_DEVICE_LORA_GW
void loop_gw()
{
  int packetSize = LoRa.parsePacket();

  if (packetSize > 0)
  {
    while (LoRa.available())
    {
      bufferVec.push_back(LoRa.read());
    }
    batch++;
  }
  if (loopEvery(M_DEVICE_LORA_BUFFER_LOOP_INTERVAL_MS) && bufferVec.size() > 0) {
    Serial1.write(bufferVec.data(), bufferVec.size());
    DEBUG("Forwarded: " << bufferVec.size() << " bytes from LoRa to COM, Batch: " << batch << endl);
    bufferVec.clear();
  }
}
#endif

void loop()
{
#ifdef M_DEVICE_LORA_GW
  loop_gw();
#elif M_DEVICE_LORA_NODE
  loop_node();
#endif
}
