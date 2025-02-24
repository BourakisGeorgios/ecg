using ECGLib.Messages.Extensions;

namespace ECGLib.Messages.Payloads;

public enum StorageStatus : byte
{
    OK = 0x1,
    NOT_OK = 0x0
}

public enum StorageType : byte
{
    UNKNOWN = 0x0,
    SD1 = 0x1,
    SD2 = 0x2,
    SDHC = 0x3,
    SDXC = 0x4,
}

public enum StorageFs : byte
{
    UNKNOWN = 0x0,
    FAT16 = 0x1,
    FAT32 = 0x2,
    EXFAT = 0x3,
}

public enum LoRaBusMode : byte
{
    Gateway = 0x0,
    Node = 0x1
}

public record StorageState(
    StorageStatus Status,
    StorageType Type,
    StorageFs Fs,
    byte ErrorCode,
    byte ErrorSymbol,
    TimeSpan InitTime,
    int TotalSize,
    int UsedSize
) : IPayload
{
    public static int GetSize() => sizeof(StorageStatus)
                                   + sizeof(StorageType)
                                   + sizeof(StorageFs)
                                   + (2 * sizeof(byte))
                                   + (3 * sizeof(int));

    public int Size => GetSize();

    public String ErrorMessage => ErrorCode switch
    {
        0 => "No error",
        1 => "Card reset failed",
        2 => "SDIO read CID",
        3 => "SDIO publish RCA",
        4 => "Switch card function",
        5 => "SDIO card select",
        6 => "Send and check interface settings",
        7 => "Read CSD data",
        8 => "Read CID data",
        9 => "Stop multiple block transmission",
        10 => "Read card status",
        11 => "Read single block",
        12 => "Read multiple blocks",
        13 => "Write single block",
        14 => "Write multiple blocks",
        15 => "Set first erase block",
        16 => "Set last erase block",
        17 => "Erase selected blocks",
        18 => "Read OCR register",
        19 => "Set CRC mode",
        20 => "Set SDIO bus width",
        21 => "Read extended status",
        22 => "Set pre-erased count",
        23 => "Activate card initialization",
        24 => "Read SCR data",
        25 => "Bad read data token",
        26 => "Read CRC error",
        27 => "SDIO fifo read timeout",
        28 => "Read CID or CSD failed.",
        29 => "Bad readStart argument",
        30 => "Read data timeout",
        31 => "Multiple block stop failed",
        32 => "SDIO transfer complete",
        33 => "Write data not accepted",
        34 => "SDIO fifo write timeout",
        35 => "Bad writeStart argument",
        36 => "Flash programming",
        37 => "Write timeout",
        38 => "DMA transfer failed",
        39 => "Card did not accept erase commands",
        40 => "Card does not support erase",
        41 => "Erase command timeout",
        42 => "Card has not been initialized",
        43 => "Invalid card config",
        44 => "Unsupported SDIO command",
        _ => "Unknown"
    };

    public byte[] GetBytes()
    {
        return
        [
            (byte)Status,
            (byte)Type,
            (byte)Fs,
            ErrorCode,
            ErrorSymbol,
            ..BitConverter.GetBytes(Convert.ToUInt32(InitTime.TotalMilliseconds)),
            ..BitConverter.GetBytes(Convert.ToUInt32(TotalSize)),
            ..BitConverter.GetBytes(Convert.ToUInt32(UsedSize)),
        ];
    }

    public static StorageState FromBytes(byte[] bytes)
    {
        return new StorageState(
            (StorageStatus)bytes[0],
            (StorageType)bytes[1],
            (StorageFs)bytes[2],
            bytes[3],
            bytes[4],
            TimeSpan.FromMilliseconds(bytes[5..(5 + sizeof(UInt32))].FromBytes<UInt32>()),
            Convert.ToInt32(bytes[9..(9 + sizeof(UInt32))].FromBytes<UInt32>()),
            Convert.ToInt32(bytes[13..(13 + sizeof(UInt32))].FromBytes<UInt32>())
        );
    }

    public override string ToString()
    {
        return
            $"{Status}: {Type} {Fs} ({(ErrorCode > 0 ? $"Error: {ErrorCode:X2}:{ErrorMessage}" : "No Error")})" +
            $"\n         {nameof(InitTime)}: {InitTime}" +
            $"\n         Size: {UsedSize}/{TotalSize}";
    }
};

public record StorageInfoPayload(bool Initialized, StorageState State) : IPayload
{
    public static int GetSize() => StorageState.GetSize() + sizeof(byte);
    public int Size => GetSize();

    public byte[] GetBytes()
    {
        return
        [
            Initialized.ToByte(),
            ..State.GetBytes()
        ];
    }

    public static StorageInfoPayload FromBytes(byte[] bytes)
    {
        return new StorageInfoPayload(
            bytes[0] == 0x1,
            StorageState.FromBytes(bytes[1..])
        );
    }

    public override string ToString()
    {
        return $"{nameof(Initialized)}: {Initialized}\n" +
               $"         {nameof(State)}: {State}";
    }
}

public record ConnectionInfoPayload(bool Initialized, bool Connected, bool IsCurrent, LoRaBusMode Mode) : IPayload
{
    public static int GetSize() => 4;

    public int Size => GetSize();

    public byte[] GetBytes()
    {
        return
        [
            Initialized.ToByte(),
            Connected.ToByte(),
            IsCurrent.ToByte(),
            (byte)Mode
        ];
    }

    public static ConnectionInfoPayload FromBytes(byte[] bytes)
    {
        return new ConnectionInfoPayload(
            bytes[0].ToBool(),
            bytes[1].ToBool(),
            bytes[2].ToBool(),
            (LoRaBusMode)bytes[3]
        );
    }

    public override string ToString()
    {
        return
            $"{nameof(Initialized)}: {Initialized}, {nameof(Connected)}: {Connected}, {nameof(IsCurrent)}: {IsCurrent}, {nameof(Mode)}: {Mode}";
    }
}

public record COMInfoPayload(bool Initialized, bool Connected, bool IsCurrent) : IPayload
{
    public static int GetSize() => 3;
    public int Size => GetSize();

    public byte[] GetBytes()
    {
        return
        [
            Initialized.ToByte(),
            Connected.ToByte(),
            IsCurrent.ToByte(),
        ];
    }

    public static COMInfoPayload FromBytes(byte[] bytes)
    {
        return new COMInfoPayload(
            bytes[0].ToBool(),
            bytes[1].ToBool(),
            bytes[2].ToBool()
        );
    }

    public override string ToString()
    {
        return
            $"{nameof(Initialized)}: {Initialized}, {nameof(Connected)}: {Connected}, {nameof(IsCurrent)}: {IsCurrent}";
    }
}

public record InfoResponsePayload(
    TimeSpan Uptime,
    IdentifyResponsePayload Info,
    StorageInfoPayload Storage,
    ConnectionInfoPayload Connection,
    COMInfoPayload COM) : IPayload
{
    public int Size => sizeof(UInt64) + Info.Size + Storage.Size + Connection.Size + COM.Size;

    public byte[] GetBytes()
    {
        return
        [
            ..BitConverter.GetBytes(Convert.ToInt64(Uptime.TotalMilliseconds)),
            ..Info.GetBytes(),
            ..Storage.GetBytes(),
            ..Connection.GetBytes(),
            ..COM.GetBytes()
        ];
    }

    public static InfoResponsePayload FromBytes(byte[] bytes)
    {
        int currIndex;
        return new InfoResponsePayload(
            TimeSpan.FromMilliseconds(bytes[0..(currIndex = sizeof(UInt64))].FromBytes<UInt64>()),
            IdentifyResponsePayload.FromBytes(bytes[currIndex..(currIndex += IdentifyResponsePayload.GetSize())]),
            StorageInfoPayload.FromBytes(bytes[currIndex..(currIndex += StorageInfoPayload.GetSize())]),
            ConnectionInfoPayload.FromBytes(bytes[currIndex..(currIndex += ConnectionInfoPayload.GetSize())]),
            COMInfoPayload.FromBytes(bytes[currIndex..(currIndex += COMInfoPayload.GetSize())])
        );
    }

    public override string ToString()
    {
        return
            $"{nameof(Uptime)}: {Uptime}" +
            $"\n{nameof(Info)}: {Info}" +
            $"\n{nameof(Storage)}: {Storage}" +
            $"\n{nameof(Connection)}: {Connection}" +
            $"\n{nameof(COM)}: {COM}";
    }
}