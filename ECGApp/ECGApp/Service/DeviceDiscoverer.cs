using System;
using System.Diagnostics;
using System.IO.Ports;
using System.Linq;
using System.Management;
using System.Threading;
using System.Threading.Tasks;
using ECGLib;
using Hangfire;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;

namespace ECGApp.Service;

public delegate Task DeviceConnected(IPortProxy portProxy, String internalDeviceId, CancellationToken ctx);

public delegate Task DeviceDisconnected(String internalDeviceId, CancellationToken ctx);

public class DeviceDiscoverer : BackgroundService
{
    private static readonly WqlEventQuery EventQuery = new()
    {
        EventClassName = "__InstanceOperationEvent",
        WithinInterval = new TimeSpan(0, 0, 3),
        Condition = @"TargetInstance ISA 'Win32_SerialPort'"
    };

    private readonly ILogger<DeviceDiscoverer> _logger;
    private readonly ManagementEventWatcher _deviceWatcher = new(EventQuery);

    public event DeviceConnected? OnDeviceConnected;
    public event DeviceDisconnected? OnDeviceDisconnected;

    public DeviceDiscoverer(ILogger<DeviceDiscoverer> logger)
    {
        this._logger = logger;
        logger.LogInformation("Device Discoverer Initializing...");
    }

    private void _deviceWatcher_EventArrived(object sender, EventArrivedEventArgs e, CancellationToken ctx)
    {
        var moBase = (ManagementBaseObject)e.NewEvent.Properties["TargetInstance"].Value!;
        var devicePnpId = moBase?.Properties["PNPDeviceID"]?.Value.ToString()!;
        bool? inserted = false;
        switch (e.NewEvent.ClassPath.ClassName)
        {
            case "__InstanceDeletionEvent":

                _logger.LogInformation("Detected a device disconnection: {deviceId}", devicePnpId);
                inserted = false;
                break;
            case "__InstanceCreationEvent":
                _logger.LogInformation("Detected a device connection: {deviceId}", devicePnpId);
                inserted = true;
                break;
            case "__InstanceModificationEvent":
                inserted = null;
                break;
            default:
                break;
        }

        if (inserted.HasValue)
        {
            HandleDeviceEvent(moBase!, inserted.Value, ctx).GetAwaiter().GetResult();
        }
    }

    private ManagementBaseObject? GetSerialPortConfig(string portName)
    {
        using var searcher =
            new ManagementObjectSearcher($"SELECT * FROM Win32_SerialPortConfiguration WHERE Name='{portName}'");
        foreach (var config in searcher.Get())
        {
            return config;
        }

        return null;
    }

    private ManagementBaseObject? GetSerialPortConfig(ManagementObject managementObject)
    {
        return managementObject
            .GetRelated("Win32_SerialPortConfiguration")
            .Cast<ManagementObject>()
            .ToList()
            .FirstOrDefault();
    }

    protected override async Task ExecuteAsync(CancellationToken stoppingToken)
    {
        _deviceWatcher.EventArrived += (sender, args)
            => _deviceWatcher_EventArrived(sender, args, stoppingToken);

        await DiscoverDevices(stoppingToken);
        _deviceWatcher.Start();
        while (!stoppingToken.IsCancellationRequested)
        {
            try
            {
                await Task.Delay(1000, stoppingToken);
            }
            catch (TaskCanceledException)
            {
            }
        }
    }

    private async Task HandleDeviceEvent(ManagementBaseObject serialDevice, bool inserted, CancellationToken ctx)
    {
        ManagementBaseObject? serialConfig = null;
        try
        {
            var pnpDeviceId = serialDevice["PNPDeviceID"]?.ToString() ?? "";
            var maxBaudRate = int.Parse(serialDevice["MaxBaudRate"]?.ToString() ?? "0");
            var name = serialDevice["DeviceID"].ToString();


            if (!inserted)
            {
                if (OnDeviceDisconnected is not null)
                {
                    await OnDeviceDisconnected(pnpDeviceId, ctx);
                }

                return;
            }

            var port = new SerialPort();

            if (serialDevice is ManagementObject managementObject)
            {
                serialConfig = GetSerialPortConfig(managementObject);
            }
            else
            {
                serialConfig = GetSerialPortConfig(serialDevice["DeviceID"]?.ToString());
            }

            if (serialConfig is null)
            {
                return;
            }

            var baudRate = serialConfig["BaudRate"]?.ToString();
            port.PortName = name;
            port.BaudRate = (baudRate is not null)
                ? int.Parse(baudRate)
                : maxBaudRate;
            port.DtrEnable = true;
            port.Parity = Parity.None;
            port.StopBits = StopBits.One;
            port.RtsEnable = true;

            if (OnDeviceConnected is not null)
            {
                await OnDeviceConnected.Invoke(new SerialPortProxy(port), pnpDeviceId, ctx);
            }
        }
        finally
        {
            serialDevice?.Dispose();
            serialConfig?.Dispose();
        }
    }

    private async Task DiscoverDevices(CancellationToken ctx = default)
    {
        try
        {
            foreach (ManagementBaseObject wmiDevice in
                     new ManagementObjectSearcher("SELECT * FROM Win32_SerialPort").Get())
            {
                _logger.LogInformation(
                    "Detected a device connection: {deviceId}",
                    wmiDevice["PNPDeviceID"]?.ToString() ?? "");
                await HandleDeviceEvent(wmiDevice, true, ctx);
            }
        }
        catch (ManagementException mex)
        {
            Debug.WriteLine(@"An error occurred while querying for WMI data: " + mex.Message);
        }
    }

    public override void Dispose()
    {
        _deviceWatcher.Stop();
        _deviceWatcher.Dispose();
        base.Dispose();
        GC.SuppressFinalize(this);
    }
}