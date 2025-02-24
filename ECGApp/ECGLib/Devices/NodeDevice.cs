using System.Collections.ObjectModel;
using ECGLib.Messages;
using ECGLib.Messages.Extensions;
using ECGLib.Messages.Payloads;

namespace ECGLib.Devices;

public class NodeDevice(IDevice parentDevice, Message originMessage) : IDevice
{
    public bool IsDisposed { get; private set; }
    public DeviceType Type => DeviceType.Node;

    public String DeviceId { get; } = originMessage.DeviceId;

    public String InternalDeviceId { get; } = Utils.GetRandomHexNumber(16);

    public IdentifyResponsePayload Identification { get; } = parentDevice.Identification with
    {
        Type = DeviceType.Node,
        Serial = new SerialNumber(originMessage.DeviceId.HexStringToByteArray())
    };

    public InfoResponsePayload DeviceInfo { get; } = new(
        TimeSpan.FromMilliseconds(originMessage.Ticks),
        parentDevice.Identification with
        {
            Type = DeviceType.Node,
            Serial = new SerialNumber(originMessage.DeviceId.HexStringToByteArray())
        },
        new StorageInfoPayload(false,
            new StorageState(StorageStatus.OK, StorageType.UNKNOWN, StorageFs.UNKNOWN, 0, 0, TimeSpan.Zero, 0, 0)),
        new ConnectionInfoPayload(true, true, true, LoRaBusMode.Node),
        new COMInfoPayload(true, false, false)
    );

    public int SubDeviceCount => 0;
    public ObservableCollection<IDevice>? SubDevices => null;

    public event DeviceMessageReceived? OnDeviceMessageReceived;

    public void Dispose()
    {
        IsDisposed = true;
    }

    public Task StartAsync(CancellationToken cancellationToken)
    {
        parentDevice.OnDeviceMessageReceived += OnMessageReceivedInternal;
        return Task.CompletedTask;
    }

    private async Task OnMessageReceivedInternal(IDevice device, Message message, CancellationToken ctx)
    {
        if (message.DeviceId == DeviceId)
        {
            if (OnDeviceMessageReceived != null)
            {
                await OnDeviceMessageReceived.Invoke(device, message, ctx);
            }
        }
    }

    public Task StopAsync(CancellationToken cancellationToken)
    {
        parentDevice.OnDeviceMessageReceived -= OnMessageReceivedInternal;
        return Task.CompletedTask;
    }
}