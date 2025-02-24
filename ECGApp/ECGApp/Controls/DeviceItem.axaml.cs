using System;
using Avalonia;
using Avalonia.Controls.Primitives;
using ECGLib.Devices;
using ECGLib.Messages;

namespace ECGApp.Controls;

public class DeviceItem : TemplatedControl
{
    public static readonly StyledProperty<IDevice> DeviceProperty =
        AvaloniaProperty.Register<DeviceItem, IDevice>(nameof(DeviceType), new FakeDevice(DeviceType.Node));

    public static readonly StyledProperty<DeviceType> DebugDeviceProperty =
        AvaloniaProperty.Register<DeviceItem, DeviceType>(nameof(DeviceType), DeviceType.Controller);

    public IDevice Device
    {
        get => GetValue(DeviceProperty);
        set => SetValue(DeviceProperty, value);
    }

    public DeviceType DebugDevice
    {
        get => GetValue(DebugDeviceProperty);
        set
        {
            SetValue(DebugDeviceProperty, value);
            SetValue(DeviceProperty, new FakeDevice(value));
        }
    }

    public string DeviceIcon => Device.Type switch
    {
        DeviceType.Controller => "mdi-antenna",
        DeviceType.Node => "mdi-raspberry-pi",
        DeviceType.Gateway => "mdi-desktop-classic",
        _ => "mdi-alert-circle"
    };

    public int? SubDeviceCount => Device.Type == DeviceType.Controller
        ? Device.SubDeviceCount
        : null;

    public DeviceType Type => Device.Type;
    public Boolean HasDevices => Device.Type == DeviceType.Controller;
}