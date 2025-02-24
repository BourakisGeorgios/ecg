using System.Collections.ObjectModel;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using CommunityToolkit.Mvvm.ComponentModel;
using ECGLib.Devices;
using ECGLib.Messages;

namespace ECGApp.ViewModels;

public partial class GatewayDeviceViewModel : ViewModelBase
{
    [ObservableProperty]
    private IDevice _device;

    private ObservableCollection<SubDeviceViewModel> _devicesVms = new();

    public ObservableCollection<SubDeviceViewModel> DeviceVms
    {
        get => _devicesVms;
        set => SetProperty(ref _devicesVms, value);
    }

    public GatewayDeviceViewModel()
    {
        PropertyChanged += (_, args) =>
        {
            if (args.PropertyName == nameof(Device))
            {
                Device.OnDeviceMessageReceived += OnMessageReceived;
            }
        };
    }

    private async Task OnMessageReceived(IDevice device, Message message, CancellationToken ctx)
    {
        var foundDeviceVm = _devicesVms.FirstOrDefault(x => x.Device.DeviceId == message.DeviceId);
        if (foundDeviceVm == null)
        {
            var nodeDevice = new NodeDevice(device, message);
            var deviceVm = new SubDeviceViewModel(nodeDevice);
            DeviceVms.Add(deviceVm);
            await deviceVm.OnMessageReceived(device, message, ctx);
            await nodeDevice.StartAsync(ctx);
        }
    }
}