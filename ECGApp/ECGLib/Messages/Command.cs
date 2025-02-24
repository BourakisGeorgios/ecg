namespace ECGLib.Messages;

public enum Command : byte
{
    Unkn = 0x0,
    IdentifyRequest = 0x1,
    IdentifyResponse = 0x2,
    TimeRequest = 0x3,
    TimeResponse = 0x4,
    InfoRequest = 0x5,
    InfoResponse = 0x6,
    Debug = 0x7,
    EcgRaw = 0x8,
    EcgElectrodeAttached = 0x9,
    EcgBpm = 0xA,
    Log = 0xB,
    Discover = 0xC,
    Connect = 0xD,
    Disconnect = 0xE,
    Ping = 0xF,
    Pong = 0x10,
    Total
}

public enum DeviceType : byte
{
    Unknown = 0x0,
    Node = 0x1,
    Controller = 0x2,
    Gateway = 0x3,
}