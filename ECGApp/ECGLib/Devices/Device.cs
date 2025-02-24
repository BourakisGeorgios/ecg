using System.Collections.ObjectModel;
using ECGLib.Messages;
using ECGLib.Messages.Payloads;
using ECGLib.Service.Messages;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;

namespace ECGLib.Devices;

public delegate Task DeviceMessageReceived(IDevice device, Message message, CancellationToken ctx = default);

public class Device(IdentifyResponsePayload identification,
                    InfoResponsePayload deviceInfo,
                    string internalDeviceId,
                    ILogger<Device> logger,
                    IMessageReader messageReader,
                    IDeviceManager manager) : BackgroundService, IDevice
{
    public Boolean IsDisposed { get; private set; }
    public IdentifyResponsePayload Identification { get; } = identification;
    public InfoResponsePayload DeviceInfo { get; } = deviceInfo;
    public DeviceType Type { get; } = identification.Type;
    public string DeviceId { get; } = identification.Serial.ToString();
    public string InternalDeviceId { get; } = internalDeviceId;

    public int SubDeviceCount { get; private set; }
    public ObservableCollection<IDevice> SubDevices { get; } = new();
    
    public List<EcgBpmPayload> EcgBpmPayloads { get; } = new();

    public event DeviceMessageReceived? OnDeviceMessageReceived;
    
    
    protected virtual async Task HandleMessage(Message message, CancellationToken ctx = default)
    {
        // manager.LogMessage(this, message, MessageDirection.Inbound);
        if (OnDeviceMessageReceived != null)
            await OnDeviceMessageReceived.Invoke(this, message, ctx);
    }

    protected override async Task ExecuteAsync(CancellationToken stoppingToken)
    {
        try
        {
            while (!stoppingToken.IsCancellationRequested)
            {
                var messageRead = await messageReader.ReadNextMessage(stoppingToken);
                if (messageRead is not null)
                {
                    await HandleMessage(messageRead, stoppingToken);
                }
            }
        }
        catch (Exception e)
        {
            logger.LogError(e, "Error in device: {deviceId} message loop", DeviceId);
            Dispose();
            throw;
        }
    }

    public override void Dispose()
    {
        IsDisposed = true;
        messageReader.Dispose();
        base.Dispose();
    }
}