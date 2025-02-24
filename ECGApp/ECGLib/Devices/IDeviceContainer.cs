using ECGLib.Messages;

namespace ECGLib.Devices;

public interface IDeviceContainer : IDevice
{
    int SubDevicesCount { get; }
}