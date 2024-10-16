// #include <Arduino.h>
// #include "config.h"
// #include "sdios.h"
// #include "Device.h"
// #include "Callback.h"
// #include "timer/InaccurateTimer.h"
// #include "timer/Timer.h"

// #include "modules/ECG.h"
// #include <functional>

// static ArduinoOutStream cout(Serial);

// struct TestCtx
// {
//   int myNum;
//   bool myBool;
//   double myDouble;
//   String myStr;
// };
// void printCtx(const String &target, TestCtx *ctx = nullptr)
// {
//   cout << target;
//   if (ctx != nullptr)
//   {
//     cout << "myNum:" << ctx->myNum << " | ";
//     cout << "myBool:" << ctx->myBool << " | ";
//     cout << "myDouble:" << ctx->myDouble << " | ";
//     cout << "myStr:" << ctx->myStr << " | ";
//   }
//   cout << endl;
// }
// class Z
// {

// public:
//   void readBpm(double a) {
//     cout << "BPM Is: " << a;
//   }
//   void test()
//   {
//     printCtx("Z::test()");
//   }
//   void test2(TestCtx *ctx)
//   {
//     printCtx("Z::test(ctx)", ctx);
//   }
// };

// String s;
// StringPrint sp(s);
// ArduinoOutStream sout(sp);
// volatile bool rdy = false;

// TestCtx *ctx_o = new TestCtx({
//     .myNum = 5,
//     .myBool = true,
//     .myDouble = 1.1,
//     .myStr = "hi",
// });
// Z *z = new Z();

// void takis(double bpm)
// {
// }

// Z a;
// typedef void (*ECGATCFUNC)(double);

// void calculateBpm(std::function<void(double)> callback) {

// }


// void test2(TestCtx *ctx)
// {
//   ::rdy = true;
//   // s = F("test2(ctx): ") + F(ctx->myNum) + F('\n');
//   ctx->myNum++;
// }

// void test()
// {
//   printCtx("::test()");
//   // cout << "OTHER TIMER: " << ctx_o->myNum << endl;
// }

// void test(int i)
// {
//   cout << "HI: " << i << endl;
// }

// void test(TestCtx *ctx)
// {
//   printCtx("::test(ctx)", ctx);
// }

// void onReceive(void (*callback)(int))
// {
//   callback(5);
// }

// Callback cb1(test);
// Callback cb2(test, ctx_o);
// Callback cb3(z, &Z::test2, ctx_o);
// Callback cb4(z, &Z::test);


// InaccurateTimer i_timer(1000, new Callback(test));
// Timer timer(200, new Callback(test2, ctx_o));

// void setup()
// {
  
//   Z a;
//   calculateBpm(takis);
//   calculateBpm(std::bind(&Z::readBpm, a, std::placeholders::_1));


//   a.test();

//   Serial.begin(9600);
//   cb1.invoke();
//   cb2.invoke();
//   cb3.invoke();
//   cb4.invoke();
//   if (!timer.start())
//   {
//     cout << "ERROR!" << endl;
//   }
//   i_timer.start();
//   // Device.init();

//   // RTCTime time(30, Month::JUNE, 2023, 13, 37, 00, DayOfWeek::WEDNESDAY, SaveLight::SAVING_TIME_ACTIVE);
//   // RTC.setTime(time);
// }

// void loop()
// {
//   timer.loop();
//   i_timer.loop();
//   if (rdy)
//   {
//     rdy = false;
//     cout << ctx_o->myNum << endl;
//   }
//   // while (Serial.available()) {
//   //   Serial.write(Serial.read());
//   // }
//   // Device.loop();
//   // RTCTime time;
//   // RTC.getTime(time);

//   // cout << "LTime:" << RTC.isRunning() << ": " << time.toString() << endl;
//   // cout << "UTime:" << time.getUnixTime() << endl;
//   // delay(1000);
// }

// // #include <SPI.h>
// // #include <SdFat.h>
// // #include "sdios.h"

// // #define P_BLACKBOX_SD_CS 4
// // const int8_t DISABLE_CS_PIN = 10;
// // #define SFN_PATH "hello.bin"

// // // Try to select the best SD card configuration.
// // // #if HAS_SDIO_CLASS
// // // #define SD_CONFIG SdioConfig(FIFO_SDIO)
// // // #elif ENABLE_DEDICATED_SPI
// // // #define SD_CONFIG SdSpiConfig(P_BLACKBOX_SD_CS, DEDICATED_SPI, SD_SCK_MHZ(16))
// // // #else  // HAS_SDIO_CLASS
// // #define SD_CONFIG SdSpiConfig(P_BLACKBOX_SD_CS, SHARED_SPI, SD_SCK_MHZ(16))
// // // #endif  // HAS_SDIO_CLASS

// // //------------------------------------------------------------------------------
// // SdFs sd;
// // cid_t cid;
// // csd_t csd;
// // scr_t scr;
// // uint8_t cmd6Data[64];
// // uint32_t eraseSize;
// // uint32_t ocr;
// // static ArduinoOutStream cout(Serial);

// // void hexDmp(void* reg, uint8_t size);

// // //------------------------------------------------------------------------------
// // void cidDmp() {
// //   byte a[] = {(byte)32, (byte)65, (byte)66, (byte)67, (byte)32};
// //   cout << a << endl << endl;
// //   cout << F("\nManufacturer ID: ");
// //   cout << uppercase << showbase << hex << int(cid.mid) << dec << endl;
// //   cout << F("OEM ID: ") << cid.oid[0] << cid.oid[1] << endl;
// //   cout << F("Product: ");
// //   for (uint8_t i = 0; i < 5; i++) {
// //     cout << cid.pnm[i];
// //   }
// //   cout << F("\nRevision: ") << cid.prvN() << '.' << cid.prvM() << endl;
// //   cout << F("Serial number: ") << hex << cid.psn() << dec << endl;
// //   cout << F("Manufacturing date: ");
// //   cout << cid.mdtMonth() << '/' << cid.mdtYear() << endl;
// //   cout << F("CID HEX: ");
// //   hexDmp(&cid, sizeof(cid));
// // }
// // //------------------------------------------------------------------------------
// // void clearSerialInput() {
// //   uint32_t m = micros();
// //   do {
// //     if (Serial.read() >= 0) {
// //       m = micros();
// //     }
// //   } while (micros() - m < 10000);
// // }
// // //------------------------------------------------------------------------------
// // void csdDmp() {
// //   eraseSize = csd.eraseSize();
// //   cout << F("\ncardSize: ") << 0.000512 * csd.capacity();
// //   cout << F(" MB (MB = 1,000,000 bytes)\n");

// //   cout << F("flashEraseSize: ") << int(eraseSize) << F(" blocks\n");
// //   cout << F("eraseSingleBlock: ");
// //   if (csd.eraseSingleBlock()) {
// //     cout << F("true\n");
// //   } else {
// //     cout << F("false\n");
// //   }
// //   cout << F("dataAfterErase: ");
// //   if (scr.dataAfterErase()) {
// //     cout << F("ones\n");
// //   } else {
// //     cout << F("zeros\n");
// //   }
// //   cout << F("CSD HEX: ");
// //   hexDmp(&csd, sizeof(csd));
// // }
// // //------------------------------------------------------------------------------
// // void errorPrint() {
// //   if (sd.sdErrorCode()) {
// //     cout << F("SD errorCode: ") << hex << showbase;
// //     printSdErrorSymbol(&Serial, sd.sdErrorCode());
// //     cout << F(" = ") << int(sd.sdErrorCode()) << endl;
// //     cout << F("SD errorData = ") << int(sd.sdErrorData()) << dec << endl;
// //   }
// // }
// // //------------------------------------------------------------------------------
// // void hexDmp(void* reg, uint8_t size) {
// //   uint8_t* u8 = reinterpret_cast<uint8_t*>(reg);
// //   cout << hex << noshowbase;
// //   for (size_t i = 0; i < size; i++) {
// //     cout << setw(2) << setfill('0') << int(u8[i]);
// //   }
// //   cout << dec << endl;
// // }
// // //------------------------------------------------------------------------------
// // bool mbrDmp() {
// //   MbrSector_t mbr;
// //   bool valid = true;
// //   if (!sd.card()->readSector(0, (uint8_t*)&mbr)) {
// //     cout << F("\nread MBR failed.\n");
// //     errorPrint();
// //     return false;
// //   }
// //   cout << F("\nSD Partition Table\n");
// //   cout << F("part,boot,bgnCHS[3],type,endCHS[3],start,length\n");
// //   for (uint8_t ip = 1; ip < 5; ip++) {
// //     MbrPart_t* pt = &mbr.part[ip - 1];
// //     if ((pt->boot != 0 && pt->boot != 0X80) ||
// //         getLe32(pt->relativeSectors) > csd.capacity()) {
// //       valid = false;
// //     }
// //     cout << int(ip) << ',' << uppercase << showbase << hex;
// //     cout << int(pt->boot) << ',';
// //     for (int i = 0; i < 3; i++) {
// //       cout << int(pt->beginCHS[i]) << ',';
// //     }
// //     cout << int(pt->type) << ',';
// //     for (int i = 0; i < 3; i++) {
// //       cout << int(pt->endCHS[i]) << ',';
// //     }
// //     cout << dec << getLe32(pt->relativeSectors) << ',';
// //     cout << getLe32(pt->totalSectors) << endl;
// //   }
// //   if (!valid) {
// //     cout << F("\nMBR not valid, assuming Super Floppy format.\n");
// //   }
// //   return true;
// // }
// // //------------------------------------------------------------------------------
// // void dmpVol() {
// //   cout << F("\nScanning FAT, please wait.\n");
// //   int32_t freeClusterCount = sd.freeClusterCount();
// //   if (sd.fatType() <= 32) {
// //     cout << F("\nVolume is FAT") << int(sd.fatType()) << endl;
// //   } else {
// //     cout << F("\nVolume is exFAT\n");
// //   }
// //   cout << F("sectorsPerCluster: ") << sd.sectorsPerCluster() << endl;
// //   cout << F("fatStartSector:    ") << sd.fatStartSector() << endl;
// //   cout << F("dataStartSector:   ") << sd.dataStartSector() << endl;
// //   cout << F("clusterCount:      ") << sd.clusterCount() << endl;
// //   cout << F("freeClusterCount:  ");
// //   if (freeClusterCount >= 0) {
// //     cout << freeClusterCount << endl;
// //   } else {
// //     cout << F("failed\n");
// //     errorPrint();
// //   }
// // }
// // //------------------------------------------------------------------------------
// // void printCardType() {
// //   cout << F("\nCard type: ");

// //   switch (sd.card()->type()) {
// //     case SD_CARD_TYPE_SD1:
// //       cout << F("SD1\n");
// //       break;

// //     case SD_CARD_TYPE_SD2:
// //       cout << F("SD2\n");
// //       break;

// //     case SD_CARD_TYPE_SDHC:
// //       if (csd.capacity() < 70000000) {
// //         cout << F("SDHC\n");
// //       } else {
// //         cout << F("SDXC\n");
// //       }
// //       break;

// //     default:
// //       cout << F("Unknown\n");
// //   }
// // }
// // //------------------------------------------------------------------------------
// // void printConfig(SdSpiConfig config) {
// //   if (DISABLE_CS_PIN < 0) {
// //     cout << F(
// //         "\nAssuming the SD is the only SPI device.\n"
// //         "Edit DISABLE_CS_PIN to disable an SPI device.\n");
// //   } else {
// //     cout << F("\nDisabling SPI device on pin ");
// //     cout << int(DISABLE_CS_PIN) << endl;
// //     pinMode(DISABLE_CS_PIN, OUTPUT);
// //     digitalWrite(DISABLE_CS_PIN, HIGH);
// //   }
// //   cout << F("\nAssuming the SD chip select pin is: ") << int(config.csPin);
// //   cout << F("\nEdit SD_CS_PIN to change the SD chip select pin.\n");
// // }
// // //------------------------------------------------------------------------------
// // void printConfig(SdioConfig config) {
// //   (void)config;
// //   cout << F("Assuming an SDIO interface.\n");
// // }
// // //-----------------------------------------------------------------------------
// // void setup() {
// //   Serial.begin(9600);
// //   // Wait for USB Serial
// //   while (!Serial) {
// //     yield();
// //   }
// //   cout << F("SdFat version: ") << SD_FAT_VERSION_STR << endl;
// //   printConfig(SD_CONFIG);
// // }
// // //------------------------------------------------------------------------------
// // void loop() {
// //   // Read any existing Serial data.
// //   clearSerialInput();

// //   // F stores strings in flash to save RAM
// //   cout << F("\ntype any character to start\n");
// //   while (!Serial.available()) {
// //     yield();
// //   }
// //   uint32_t t = millis();
// //   if (!sd.cardBegin(SD_CONFIG)) {
// //     cout << F(
// //         "\nSD initialization failed.\n"
// //         "Do not reformat the card!\n"
// //         "Is the card correctly inserted?\n"
// //         "Is there a wiring/soldering problem?\n");
// //     if (isSpi(SD_CONFIG)) {
// //       cout << F(
// //           "Is SD_CS_PIN set to the correct value?\n"
// //           "Does another SPI device need to be disabled?\n");
// //     }
// //     errorPrint();
// //     return;
// //   }
// //   t = millis() - t;
// //   cout << F("init time: ") << dec << t << " ms" << endl;

// //   if (!sd.card()->readCID(&cid) || !sd.card()->readCSD(&csd) ||
// //       !sd.card()->readOCR(&ocr) || !sd.card()->readSCR(&scr)) {
// //     cout << F("readInfo failed\n");
// //     errorPrint();
// //     return;
// //   }
// //   printCardType();
// //   cout << F("sdSpecVer: ") << 0.01 * scr.sdSpecVer() << endl;
// //   cout << F("HighSpeedMode: ");
// //   if (scr.sdSpecVer() > 101 && sd.card()->cardCMD6(0X00FFFFFF, cmd6Data) &&
// //       (2 & cmd6Data[13])) {
// //     cout << F("true\n");
// //   } else {
// //     cout << F("false\n");
// //   }
// //   cidDmp();
// //   csdDmp();
// //   cout << F("\nOCR: ") << uppercase << showbase;
// //   cout << hex << ocr << dec << endl;
// //   if (!mbrDmp()) {
// //     return;
// //   }
// //   if (!sd.volumeBegin()) {
// //     cout << F("\nvolumeBegin failed. Is the card formatted?\n");
// //     errorPrint();
// //     return;
// //   }
// //   dmpVol();
// //   sd.ls(LS_R);
// // }