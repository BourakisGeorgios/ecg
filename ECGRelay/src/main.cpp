#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>
#include <sdios.h>

#define M_DEVICE_LORA_DEFAULT_FREQ 868E6
#define M_DEVICE_LORA_NSS_PIN 8
#define M_DEVICE_LORA_NRESET_PIN 9
#define M_DEVICE_LORA_DIO0_PIN 2

#if defined(M_DEVICE_LORA_GW) && defined(M_DEVICE_LORA_NODE)
#error The device should act as a gateway or node, not both.
#elif !defined(M_DEVICE_LORA_GW) && !defined(M_DEVICE_LORA_NODE)
#error The device should act as a gateway or node, none defined. \
       Conider defining either M_DEVICE_LORA_NODE or M_DEVICE_LORA_GW
#endif

ArduinoOutStream cout(Serial);

#define DEBUG(x) do { \
  if (Serial) { cout << x; } \
} while (0)


bool initialized = false;
bool reportedStatus = false;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);

  delay(1000);

  if (Serial) {
    cout << "Initializing LoRa Relay." << endl;
  }

  LoRa.setPins(M_DEVICE_LORA_NSS_PIN, M_DEVICE_LORA_NRESET_PIN, M_DEVICE_LORA_DIO0_PIN);
  initialized = !LoRa.begin(M_DEVICE_LORA_DEFAULT_FREQ);


  Serial1.write((byte)(initialized ? 0x1 : 0x0));

  if (!initialized)
  {
    while (true)
    {
      delay(500);
      DEBUG("LoRa Antenna failed to initialize. " << endl);
    }
  }

  DEBUG("LoRa Antenna initialized. " << endl);

#ifdef M_DEVICE_LORA_GW
  LoRa.receive();
  DEBUG("LoRa Antenna set to receive only mode. " << endl);
#endif
}

#ifdef M_DEVICE_LORA_NODE
void loop_node()
{
  byte *buffer;
  int received = Serial1.available();
  if (received > 0)
  {
    buffer = new byte[received];

    while (Serial1.available())
    {
      Serial1.readBytes(buffer, received);
    }
    LoRa.write(buffer, received);

    DEBUG("Forwarded: " << received << " bytes from COM to LoRa" << endl);

    delete[] buffer;
  }
}
#endif

#ifdef M_DEVICE_LORA_GW
void loop_gw()
{
  if (LoRa.parsePacket())
  {
    byte *buffer;
    int received = LoRa.available();
    if (received > 0)
    {
      buffer = new byte[received];

      while (LoRa.available())
      {
        LoRa.readBytes(buffer, received);
      }

      Serial1.write(buffer, received);
      
      DEBUG("Forwarded: " << received << " bytes from LoRa to COM" << endl);

      delete[] buffer;
    }
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
