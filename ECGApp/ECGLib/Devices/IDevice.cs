using System.Collections.ObjectModel;
using ECGLib.Messages;
using ECGLib.Messages.Payloads;
using Microsoft.Extensions.Hosting;

namespace ECGLib.Devices;

public interface IDevice : IHostedService, IDisposable
{
    Boolean IsDisposed { get; }
    DeviceType Type { get; }
    
    String DeviceId { get; }
    
    String InternalDeviceId { get; }
    
    IdentifyResponsePayload Identification { get; }
    
    InfoResponsePayload DeviceInfo { get; }
    
    int SubDeviceCount { get; }
    
    ObservableCollection<IDevice>? SubDevices { get; }
    
    public event DeviceMessageReceived? OnDeviceMessageReceived;
}