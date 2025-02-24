using ECGLib.Messages.Extensions;

namespace ECGLib.Messages.Payloads;

public record EcgBpmPayload(double BeatsPerMinute) : IPayload
{
    public static int GetSize() => sizeof(double);
    public int Size => GetSize();

    public byte[] GetBytes()
    {
        return
        [
            ..BitConverter.GetBytes(Convert.ToDouble(BeatsPerMinute))
        ];
    }

    public static EcgBpmPayload FromBytes(byte[] bytes)
    {
        return new EcgBpmPayload(bytes.FromBytes<double>());
    }
}