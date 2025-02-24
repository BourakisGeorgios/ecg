using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Security.Cryptography;
using System.Threading.Tasks;
using CommunityToolkit.Mvvm.ComponentModel;
using LiveChartsCore;
using LiveChartsCore.Defaults;
using LiveChartsCore.SkiaSharpView;
using LiveChartsCore.SkiaSharpView.Painting;
using LiveChartsCore.SkiaSharpView.Painting.Effects;
using SkiaSharp;

namespace ECGApp.ViewModels;

public partial class FakeNodeDeviceViewModel : ViewModelBase
{
    private readonly int[] _refValues;
    private int _refValueIndex = 0;
    private readonly List<DateTimePoint> _values = [];
    private readonly DateTimeAxis _customAxis;

    public ObservableCollection<ISeries> Series { get; set; }

    public Axis[] XAxes { get; set; }
    public Axis[] YAxes { get; set; }


    [ObservableProperty]
    private object _sync = new object();

    [ObservableProperty]
    private bool _isReading = true;

    public FakeNodeDeviceViewModel()
    {
        _refValues =
        [
            513, 511, 513, 511, 513, 511, 513, 511, 515, 518, 531, 536, 536, 522, 518, 510, 515, 507, 492, 471, 512,
            513, 704, 679, 506, 507, 457, 490, 507, 515, 509, 514, 510, 514, 511, 515, 514, 520, 522, 532, 540, 555,
            566, 580, 586, 590, 584, 577, 563, 552, 538, 530, 521, 518, 514, 514, 512, 513, 511, 513, 511, 513, 511,
            513, 511, 513, 511, 513, 511, 513, 511, 513, 511, 513, 511
        ];
        Series =
        [
            new LineSeries<DateTimePoint>
            {
                Values = _values,
                Fill = null,
                GeometryFill = null,
                GeometryStroke = null,
                Stroke = new SolidColorPaint(SKColor.Parse("#63c531"), 2)
            }
        ];

        _customAxis = new DateTimeAxis(TimeSpan.FromSeconds(1), Formatter)
        {
            // MaxLimit = _refValues.Length * 3,
            CustomSeparators = GetSeparators(),
            AnimationsSpeed = TimeSpan.FromMilliseconds(0),
            // SeparatorsPaint = new SolidColorPaint(SKColors.Black.WithAlpha(100)),
            SeparatorsPaint = new SolidColorPaint
            {
                Color = SKColors.Black.WithAlpha(100),
                StrokeThickness = 1,
                PathEffect = new DashEffect([3, 3])
            },
            SubseparatorsPaint = new SolidColorPaint
            {
                Color = SKColors.Black.WithAlpha(100),
                StrokeThickness = 0.5f
            },
            SubseparatorsCount = 5,
            ZeroPaint = new SolidColorPaint
            {
                Color = SKColors.Black.WithAlpha(100),
                StrokeThickness = 2
            },
            TicksPaint = new SolidColorPaint
            {
                Color = SKColors.Black.WithAlpha(100),
                StrokeThickness = 1.5f
            },
            SubticksPaint = new SolidColorPaint
            {
                Color = SKColors.Black.WithAlpha(100),
                StrokeThickness = 1
            }
        };

        XAxes = [_customAxis];
        YAxes =
        [
            new Axis()
            {
                Labels = new List<String>(),
                MaxLimit = 1024,
                MinLimit = 0,
                SeparatorsPaint = new SolidColorPaint
                {
                    Color = SKColors.Black.WithAlpha(100),
                    StrokeThickness = 1,
                    PathEffect = new DashEffect([3, 3])
                },
                SubseparatorsPaint = new SolidColorPaint
                {
                    Color = SKColors.Black.WithAlpha(100),
                    StrokeThickness = 0.5f
                },
                SubseparatorsCount = 5,
                ZeroPaint = new SolidColorPaint
                {
                    Color = SKColors.Black.WithAlpha(100),
                    StrokeThickness = 2
                },
                TicksPaint = new SolidColorPaint
                {
                    Color = SKColors.Black.WithAlpha(100),
                    StrokeThickness = 1.5f
                },
                SubticksPaint = new SolidColorPaint
                {
                    Color = SKColors.Black.WithAlpha(100),
                    StrokeThickness = 1
                }
            }
        ];

        _ = ReadData();
    }

    private async Task ReadData()
    {
        var rng = RandomNumberGenerator.Create();

        // to keep this sample simple, we run the next infinite loop 
        // in a real application you should stop the loop/task when the view is disposed 
        // _values.AddRange(Enumerable.Range(0, (_refValues.Length * 5))
        //     .Select(x => new DateTimePoint(DateTime.Now
        //         .Subtract(TimeSpan.FromMilliseconds(10 * x)), 512))
        //     .ToList());
        // Console.WriteLine(_values/**/.Count);
        // Console.WriteLine(_refValues.Length * 5);
        while (IsReading)
        {
            await Task.Delay(TimeSpan.FromMilliseconds(10));

            // Because we are updating the chart from a different thread 
            // we need to use a lock to access the chart data. 
            // this is not necessary if your changes are made on the UI thread. 

            var dt = DateTime.Now;
            Task.Run(() =>
            {
                lock (Sync)
                {
                    _values.Add(new DateTimePoint(dt,
                        _refValues[_refValueIndex]));
                    _refValueIndex = (_refValueIndex + 1) % _refValues.Length;
                    if (_values.Count > _refValues.Length * 5)
                    {
                        _values.RemoveAt(0);
                    }
                }
            });


            // we need to update the separators every time we add a new point 
            _customAxis.CustomSeparators = GetSeparators();
        }
    }

    private static double[] GetSeparators()
    {
        var now = DateTime.Now;

        return
        [
            now.AddSeconds(-25).Ticks,
            now.AddSeconds(-20).Ticks,
            now.AddSeconds(-15).Ticks,
            now.AddSeconds(-10).Ticks,
            now.AddSeconds(-5).Ticks,
            now.Ticks
        ];
    }

    private static string Formatter(DateTime date)
    {
        return String.Empty;
        var secsAgo = (DateTime.Now - date).TotalSeconds;

        return secsAgo < 1
            ? "now"
            : $"{secsAgo:N0}s ago";
    }
}