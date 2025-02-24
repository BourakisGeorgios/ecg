using System.Text;

namespace ECGLib.Messages.Payloads;

public record Version(int Major, int Minor, int Revision) : IPayload
{
    public static int GetSize() => 3;
    
    public int Size => GetSize();

    public byte[] GetBytes()
    {
        return
        [
            (byte)(Major >> 8),
            (byte)(Minor >> 8),
            (byte)(Revision >> 8)
        ];
    }

    public override string ToString()
    {
        return $"{Major}.{Minor}.{Revision}";
    }

    public static Version FromBytes(byte[] bytes)
    {
        return new Version(bytes[0], bytes[1], bytes[2]);
    }
}

public record SerialNumber(byte[] Bytes) : IPayload
{
    public static int GetSize() => 16;
    public int Size => Bytes.Length;

    public byte[] GetBytes()
    {
        return Bytes;
    }

    public override string ToString()
    {
        return string.Join("", Bytes.Select(x => $"{x:X2}"));
    }
}

public record IdentifyResponsePayload(string Name, Version Version, DeviceType Type, SerialNumber Serial) : IPayload
{
    public static int GetSize() => 5 + Version.GetSize() + sizeof(byte) + SerialNumber.GetSize();
    public int Size => 5 + Version.Size + sizeof(byte) + Serial.Size;

    public byte[] GetBytes()
    {
        return
        [
            ..Encoding.ASCII.GetBytes(Name[..4] + '\0'),
            ..Version.GetBytes(),
            (byte)Type,
            ..Serial.GetBytes()
        ];
    }

    public static IdentifyResponsePayload FromBytes(byte[] bytes)
    {
        var name = Encoding.ASCII.GetString(bytes![0..4]);
        var deviceTypeByte = bytes[5];
        var deviceType = deviceTypeByte > 0x3
            ? DeviceType.Unknown
            : (DeviceType)deviceTypeByte;
        var version = Version.FromBytes(bytes[6..9]);
        var serialNumbers = new SerialNumber(bytes[9..25]);
        return new IdentifyResponsePayload(name, version, deviceType, serialNumbers);
    }

    public override string ToString()
    {
        return $"{Name} {Version} [{Type}] {Serial}";
    }
}