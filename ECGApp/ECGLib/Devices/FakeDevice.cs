using System.Collections.ObjectModel;
using ECGLib.Messages;
using ECGLib.Messages.Payloads;

namespace ECGLib.Devices;

public class FakeDevice(DeviceType deviceType) : IDevice
{
    public ObservableCollection<IDevice> SubDevices { get; } = new();
    public event DeviceMessageReceived? OnDeviceMessageReceived;



    public bool IsDisposed { get; } = false;
    public DeviceType Type => deviceType;

    public String DeviceId { get; } = Utils.GetRandomHexNumber(16);
    
    public String InternalDeviceId { get; } = Utils.GetRandomHexNumber(16);
    public IdentifyResponsePayload Identification { get; }
    public InfoResponsePayload DeviceInfo { get; }
    public int SubDeviceCount { get; } = 0;

    public void Dispose()
    {
        // TODO release managed resources here
    }

    public Task StartAsync(CancellationToken cancellationToken)
    {
        return Task.CompletedTask;
        // throw new NotImplementedException();
    }

    public Task StopAsync(CancellationToken cancellationToken)
    {
        return Task.CompletedTask;
        // throw new NotImplementedException();
    }
}