using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Avalonia.Threading;
using CommunityToolkit.Mvvm.ComponentModel;
using ECGLib.Devices;
using ECGLib.Messages;
using ECGLib.Messages.Payloads;
using LiveChartsCore;
using LiveChartsCore.Defaults;
using LiveChartsCore.SkiaSharpView;
using LiveChartsCore.SkiaSharpView.Painting;
using SkiaSharp;

namespace ECGApp.ViewModels;

public partial class NodeDeviceViewModel : ViewModelBase
{
    private readonly int[] _refValues;
    private int _refValueIndex = 0;

    [ObservableProperty]
    private int _currentBpm;

    [ObservableProperty]
    private IDevice _device;

    public ObservableCollection<int> Values = new();
    public ObservableCollection<ISeries> Series { get; set; }

    public Axis[] XAxes { get; set; }
    public Axis[] YAxes { get; set; }


    [ObservableProperty]
    private object _sync = new object();

    [ObservableProperty]
    private bool _isReading = true;

    public NodeDeviceViewModel() : this(new FakeDevice(DeviceType.Node))
    {
        Console.WriteLine("CTORED B-A-D");
    }


    public NodeDeviceViewModel(IDevice device)
    {
        Console.WriteLine("CTORED");
        Values = new ObservableCollection<int>(Enumerable.Range(0, 75 * 5)
            .Select(x => 300));
        _device = device;
        _device.OnDeviceMessageReceived += OnMessageReceived;
        CurrentBpm = 0;

        Series = new ObservableCollection<ISeries>
        {
            new LineSeries<int>
            {
                Values = Values,
                Fill = null,
                GeometryFill = null,
                GeometryStroke = null,
                Stroke = new SolidColorPaint(SKColor.Parse("#63c531"), 2)
            }
        };
        YAxes =
        [
            new Axis()
            {
                MinLimit = 0,
                MaxLimit = 1024,
            }
        ];
    }

    private Task OnMessageReceived(IDevice device, Message message, CancellationToken ctx)
    {
        if (message.Command == Command.EcgRaw)
        {
            var payload = (EcgRawPayload)message.Payload!;
            Dispatcher.UIThread.Invoke(() =>
                {
                    Values.Add(payload.Measurement);
                    if (Values.Count > 75 * 5)
                    {
                        Values.RemoveAt(0);
                    }
                }
            );
        }
        else if (message.Command == Command.EcgBpm)
        {
            var payload = (EcgBpmPayload)message.Payload!;
            Dispatcher.UIThread.Invoke(() => { CurrentBpm = (int)Math.Ceiling(payload.BeatsPerMinute); });
        }

        return Task.CompletedTask;
    }
}