using System.Collections.ObjectModel;
using ECGLib.Messages;

namespace ECGLib.Devices;

public delegate void DeviceConnectedDelegate(IDevice device);

public delegate void DeviceDisconnectedDelegate(IDevice device);

public interface IDeviceManager
{
    ReadOnlyObservableCollection<IDevice> Devices { get; }

    event DeviceConnectedDelegate OnDeviceConnected;
    event DeviceDisconnectedDelegate OnDeviceDisconnected;

    void LogMessage(IDevice device, Message message, MessageDirection direction);
}