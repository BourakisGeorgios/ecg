using ECGLib.Messages.Extensions;
using ECGLib.Messages.Payloads;

namespace ECGLib.Messages;

public record Message(
    Command Command,
    String DeviceId,
    String TargetDeviceId,
    UInt64 Ticks,
    DateTimeOffset Time,
    IPayload? Payload = null)
{
    private static readonly int CommandPadding = Enum.GetValues<Command>()
        .Select(x => x.ToString().Length)
        .Max();

    public Task Write(Stream stream, CancellationToken ctx = default)
    {
        var messageSize = BitConverter
            .GetBytes(1 + (2 * 6 * sizeof(byte)) + (3 * sizeof(UInt32)) + sizeof(Int64) + Payload?.Size ?? 0);
        var tickBytes = BitConverter.GetBytes(Convert.ToUInt32(Ticks));
        var timeBytes = BitConverter.GetBytes(Convert.ToInt64(Time.ToUnixTimeSeconds()));
        var payloadSizeBytes = BitConverter.GetBytes(Convert.ToUInt32(Payload?.Size ?? 0));
        byte[] messageBytes =
        [
            (byte)Command,
            ..messageSize,
            ..DeviceId.HexStringToByteArray(),
            ..TargetDeviceId?.HexStringToByteArray() ?? [0, 0, 0, 0, 0, 0],
            ..tickBytes,
            ..timeBytes,
            ..payloadSizeBytes,
            ..Payload?.GetBytes() ?? []
        ];
        return stream.WriteAsync(messageBytes, ctx).AsTask();
    }

    public override string ToString()
    {
        return $"{nameof(Command)}: {Command.ToString().PadRight(CommandPadding)}, " +
               $"{nameof(DeviceId)}: {DeviceId}, " +
               $"{nameof(TargetDeviceId)}: {TargetDeviceId??"000000000000"}, " +
               $"{nameof(Ticks)}: {Ticks,-12}, " +
               $"{nameof(Time)}: {Time}";
    }
}