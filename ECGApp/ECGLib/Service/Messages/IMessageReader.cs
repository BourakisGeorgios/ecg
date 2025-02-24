using ECGLib.Messages;

namespace ECGLib.Service.Messages;

public interface IMessageReader : IDisposable
{
    Task<Message?> ReadNextMessage(CancellationToken ctx = default);
}