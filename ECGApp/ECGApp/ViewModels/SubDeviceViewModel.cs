using System;
using System.Threading;
using System.Threading.Tasks;
using CommunityToolkit.Mvvm.ComponentModel;
using ECGLib.Devices;
using ECGLib.Messages;
using ECGLib.Messages.Payloads;

namespace ECGApp.ViewModels;

public partial class SubDeviceViewModel : ViewModelBase
{
    [ObservableProperty]
    private int _currentBpm;

    [ObservableProperty]
    private IDevice _device;

    [ObservableProperty]
    private string _status = String.Empty;

    public SubDeviceViewModel(IDevice device)
    {
        Device = device;
        _device.OnDeviceMessageReceived += OnMessageReceived;
    }

    public Task OnMessageReceived(IDevice device, Message message, CancellationToken ctx)
    {
        if (message.Command == Command.EcgBpm)
        {
            var payload = (EcgBpmPayload)message.Payload!;
            CurrentBpm = (int)Math.Ceiling(payload.BeatsPerMinute);
            Status = CurrentBpm switch
            {
                >= 10 and < 60 => "Low Heartbeat",
                >= 60 and < 120 => "Normal Heartbeat",
                >= 120 and < 180 => "High Heartbeat",
                _ => "No Heartbeat",
            };
            Console.WriteLine("Updating...");
        }
        return Task.CompletedTask;
    }
}