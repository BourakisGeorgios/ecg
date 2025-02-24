using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using ECGApp.Service.Messages;
using ECGApp.ViewModels;
using ECGLib;
using ECGLib.Devices;
using ECGLib.Messages;
using ECGLib.Messages.Extensions;
using ECGLib.Messages.Payloads;
using ECGMonitor.Device.Utils;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using Version = ECGLib.Messages.Payloads.Version;

namespace ECGApp.Service;

public class DeviceManager : BackgroundService, IDeviceManager
{
    private readonly DeviceDiscoverer _deviceDiscoverer;
    private readonly DeviceValidator _deviceValidator;
    private readonly MessageHub _messageHub;
    private readonly ILoggerFactory _loggerFactory;
    private readonly ILogger<DeviceManager> _logger;
    private readonly IdentifyResponsePayload _identification;
    private readonly Dictionary<string, IPortProxy> _ports = new();
    private readonly Dictionary<string, IDevice> _deviceMap = new();
    private readonly ObservableCollection<IDevice> _devices = new();
    private readonly MainWindowViewModel _mainWindowViewModel;
    
    public event DeviceConnectedDelegate? OnDeviceConnected;
    public event DeviceDisconnectedDelegate? OnDeviceDisconnected;
    

    public ReadOnlyObservableCollection<IDevice> Devices { get; init; }

    public DeviceManager(DeviceDiscoverer deviceDiscoverer,
                         DeviceValidator deviceValidator,
                         ILogger<DeviceManager> logger,
                         MessageHub messageHub,
                         ILoggerFactory loggerFactory,
                         MainWindowViewModel mainWindowViewModel)
    {
        logger.LogInformation(Guid.NewGuid().ToString());
        Devices = new ReadOnlyObservableCollection<IDevice>(_devices);
        _deviceDiscoverer = deviceDiscoverer;
        _deviceValidator = deviceValidator;
        _logger = logger;
        _messageHub = messageHub;
        _loggerFactory = loggerFactory;
        _mainWindowViewModel = mainWindowViewModel;
        _identification = new IdentifyResponsePayload(
            "MEDI",
            new Version(0, 0, 1),
            DeviceType.Controller,
            new SerialNumber(SystemUtils.HostDeviceId[^6..])
        );
        _deviceDiscoverer.OnDeviceConnected += DeviceConnected;
        _deviceDiscoverer.OnDeviceDisconnected += DeviceDisconnected;
        _deviceValidator.OnDeviceValidated += DeviceValidated;
        _deviceValidator.OnDeviceInvalidated += DeviceInvalidated;
        _messageHub.OnMessageSent += OnMessageSent;
    }

    private Task OnMessageSent(IDevice device, Message message, CancellationToken cancellationToken)
    {
        LogMessage(device, message, MessageDirection.Outbound);
        return Task.CompletedTask;
    }

    private Task DeviceInvalidated(string deviceId, CancellationToken token)
    {
        _logger.LogWarning("Failed Initializing Device: {deviceId}", deviceId);
        return Task.CompletedTask;
    }

    private async Task DeviceValidated(string deviceId,
                                       IdentifyResponsePayload identifyResponsePayload,
                                       InfoResponsePayload infoResponsePayload,
                                       List<(Message, MessageDirection)> negotiatedMessages,
                                       MessageReader reader,
                                       IPortProxy port,
                                       CancellationToken token
    )
    {
        var device = new Device(
            identifyResponsePayload,
            infoResponsePayload,
            deviceId,
            _loggerFactory.CreateLogger<Device>(),
            reader,
            this
        );
        _deviceMap.Add(deviceId, device);
        _devices.Add(device);
        _ports.Add(deviceId, port);

        _messageHub.RegisterDevice(device, port);

        _logger.LogInformation("Creating Device {pnpDeviceId} on Port: {portName}", deviceId, port.PortName);

        foreach (var (message, direction) in negotiatedMessages)
        {
            LogMessage(device, message, direction);
        }
        
        _mainWindowViewModel.DeviceManagerOnOnDeviceConnected(device);

        await device.StartAsync(default);
    }

    public byte[] HostId { get; } = SystemUtils.HostDeviceId;

    private async Task DeviceDisconnected(string internalDeviceId, CancellationToken cancellationToken)
    {
        if (!_deviceMap.TryGetValue(internalDeviceId, out IDevice? device))
        {
            return;
        }

        _deviceMap.Remove(internalDeviceId);
        _devices.Remove(device);
        _ports.Remove(internalDeviceId);

        _messageHub.UnregisterDevice(device);
       
        _mainWindowViewModel.DeviceManagerOnOnDeviceDisconnected(device);
        

        await device.StopAsync(cancellationToken);

        device.Dispose();
    }

    private async Task DeviceConnected(IPortProxy portProxy,
                                       string internalDeviceId,
                                       CancellationToken cancellationToken)
    {
        await _deviceValidator.Submit(portProxy, internalDeviceId, cancellationToken);
    }

    public void LogMessage(IDevice device, Message message, MessageDirection direction)
    {
        var actualIdentification = direction switch
        {
            MessageDirection.Outbound => _identification,
            MessageDirection.Inbound => device.Identification,
            _ => throw new ArgumentOutOfRangeException(nameof(direction), direction, null)
        };

        var header = $"[{actualIdentification.Name} {actualIdentification.Version} " +
                     $"[{actualIdentification.Type.ToString().PadLeft(SystemUtils.DeviceTypePadding)}] " +
                     $"{actualIdentification.Serial.Bytes[^6..].GetHexString()}]: ";

        Console.WriteLine($"{header}{message}");
        if (message.Payload is not null)
        {
            var payloadHeader = $"{"".PadLeft(header.Length)}Payload: ";
            message.Payload.ToString()
                .Split("\n")
                .Select((s, i) => i == 0
                    ? $"{payloadHeader}{s}"
                    : $"{"".PadLeft(payloadHeader.Length)}{s}")
                .ToList()
                .ForEach(messageLog => Console.WriteLine(messageLog));
        }
    }

    protected override async Task ExecuteAsync(CancellationToken stoppingToken)
    {
        await _deviceDiscoverer.StartAsync(stoppingToken);
        while (!stoppingToken.IsCancellationRequested)
        {
            foreach (var device in new List<IDevice>(_devices))
            {
                if (device.IsDisposed)
                {
                    await DeviceDisconnected(device.InternalDeviceId, stoppingToken);
                }
            }

            await Task.Delay(1000, stoppingToken);
        }
    }
}