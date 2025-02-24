namespace ECGLib.Messages.Payloads;

public interface IPayload
{
    public static Dictionary<Command, Func<byte[], IPayload?>> CommandPayloadConstructors => new()
    {
        [Command.IdentifyResponse] = IdentifyResponsePayload.FromBytes,
        [Command.EcgRaw] = EcgRawPayload.FromBytes,
        [Command.EcgBpm] = EcgBpmPayload.FromBytes,
        [Command.InfoResponse] = InfoResponsePayload.FromBytes,
    };

    int Size { get; }

    byte[] GetBytes();
    
    string ToString() => nameof(IPayload);
}