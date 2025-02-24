using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using ECGLib;
using ECGLib.Devices;
using ECGLib.Messages;
using ECGLib.Messages.Extensions;
using ECGLib.Messages.Payloads;
using ECGMonitor.Device.Utils;

namespace ECGApp.Service.Messages;

public delegate Task MessageSendDelegate(IDevice device, Message message, CancellationToken cancellationToken);

public class MessageHub
{
    private readonly Dictionary<IDevice, IPortProxy> _devicePorts = new();

    public event MessageSendDelegate? OnMessageSent;

    public void RegisterDevice(IDevice device, IPortProxy port)
        => _devicePorts.Add(device, port);

    public void UnregisterDevice(IDevice device)
        => _devicePorts.Remove(device);

    public Message CreateMessage(Command command, IPayload? payload = null)
    {
        return new(
            command,
            SystemUtils.HostDeviceId[^6..].GetHexString(),
            SystemUtils.DefaultId[^6..].GetHexString(),
            SystemUtils.GetTickCount64(),
            DateTimeOffset.Now,
            payload
        );
    }

    public Task SendCommand(IDevice device,
                            Command command,
                            IPayload? payload = null,
                            CancellationToken ctx = default)
    {
        return SendCommand(device, CreateMessage(command, payload), ctx);
    }

    public async Task SendCommand(IDevice device,
                                  Message message,
                                  CancellationToken ctx = default)
    {
        var adjustedMessage = message with
        {
            TargetDeviceId = device
                .DeviceId
                .HexStringToByteArray()[^6..].GetHexString()
        };
        
        await adjustedMessage.Write(_devicePorts[device].BaseStream, ctx);

        if (OnMessageSent is not null)
        {
            await OnMessageSent(device, message, ctx);
        }
    }

    public async Task SendCommand(IPortProxy port,
                                  String? deviceId,
                                  Message message,
                                  CancellationToken ctx = default)
    {
        var adjustedMessage = message with
        {
            TargetDeviceId = deviceId
        };
        await adjustedMessage.Write(port.BaseStream, ctx);
    }
}