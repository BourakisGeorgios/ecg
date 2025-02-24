using ECGLib.Messages.Extensions;

namespace ECGLib.Messages.Payloads;

public record EcgRawPayload(int Measurement) : IPayload
{
    public static int GetSize() => sizeof(int);
    public int Size => GetSize();

    public byte[] GetBytes()
    {
        return
        [
            ..BitConverter.GetBytes(Convert.ToInt32(Measurement))
        ];
    }

    public static EcgRawPayload FromBytes(byte[] bytes)
    {
        return new EcgRawPayload(Convert.ToInt32(bytes.FromBytes<Int32>()));
    }
}