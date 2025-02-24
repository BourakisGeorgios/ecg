namespace ECGLib.Messages.Payloads;

public record TimeResponse(DateTimeOffset Time) : IPayload
{
    public int Size => sizeof(Int64);

    public byte[] GetBytes()
    {
        return
        [
            ..BitConverter.GetBytes(Convert.ToInt64(Time.ToUnixTimeSeconds()))
        ];
    }

    public override string ToString()
    {
        return $"{nameof(Time)}: {Time}";
    }
}