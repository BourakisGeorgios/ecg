using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Threading;
using System.Threading.Tasks;
using CommunityToolkit.Mvvm.ComponentModel;
using ECGApp.Service;
using ECGLib.Devices;
using ECGLib.Messages;

namespace ECGApp.ViewModels;

public partial class MainWindowViewModel : ViewModelBase
{
    [ObservableProperty]
    private int _connectedDevices;

    [ObservableProperty]
    private ViewModelBase _selectedDeviceView;

    [ObservableProperty]
    private IDevice _selectedDevice;

    private ObservableCollection<IDevice> _devices;

    public ObservableCollection<IDevice> Devices
    {
        get => _devices;
        set => SetProperty(ref _devices, value);
    }

    partial void OnSelectedDeviceChanged(IDevice? device)
    {
        if (device == null)
            return;

        switch (device.Type)
        {
            case DeviceType.Node:
                SelectedDeviceView = new NodeDeviceViewModel(device);
                break;
            case DeviceType.Gateway:
            {
                var gateway = new GatewayDeviceViewModel();
                SelectedDeviceView = gateway;
                gateway.Device = device;
                break;
            }
            default:
                break;
        }
    }

    public MainWindowViewModel()
    {
        
        _devices = new ObservableCollection<IDevice>();
        
        // Task.Run(async () =>
        // {
        //     Devices.Add(new FakeDeviceContainer(DeviceType.Controller));
        //     SelectedDevice = Devices[0];
        //     await Task.Delay(1000);
        //     Devices.Add(new FakeDevice(DeviceType.Node));
        //     await Task.Delay(1000);
        //     Devices.Add(new FakeDevice(DeviceType.Gateway));
        //     await Task.Delay(1000);
        // });
    }

    public void DeviceManagerOnOnDeviceDisconnected(IDevice device)
    {
        Devices.Remove(device);
    }

    public void DeviceManagerOnOnDeviceConnected(IDevice device)
    {
        Devices.Add(device);
    }

    public void ConnectCommand()
    {
        ConnectedDevices++;
    }
}