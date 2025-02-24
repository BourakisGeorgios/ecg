using CommunityToolkit.Mvvm.ComponentModel;

namespace ECGApp.ViewModels;

public partial class DeviceListItemViewModel : ViewModelBase
{
    [ObservableProperty]
    private int _connectedDevices = 0;
}