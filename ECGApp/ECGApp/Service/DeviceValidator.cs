using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using ECGApp.Service.Messages;
using ECGLib;
using ECGLib.Messages;
using ECGLib.Messages.Extensions;
using ECGLib.Messages.Payloads;
using ECGMonitor.Device.Utils;
using Hangfire;
using Microsoft.Extensions.Logging;

namespace ECGApp.Service;

public delegate Task DeviceValidatedDelegate(String deviceId,
                                             IdentifyResponsePayload identifyResponsePayload,
                                             InfoResponsePayload infoResponsePayload,
                                             List<(Message, MessageDirection)> negotiatedMessages,
                                             MessageReader reader,
                                             IPortProxy port,
                                             CancellationToken token);

public delegate Task DeviceInvalidatedDelegate(String deviceId,
                                               CancellationToken token);

public class DeviceValidator(MessageHub messageHub, ILogger<DeviceValidator> logger)
{
    private readonly Dictionary<string, Task> _activeTasks = new();

    public event DeviceValidatedDelegate? OnDeviceValidated;
    public event DeviceInvalidatedDelegate? OnDeviceInvalidated;

    public Task Submit(IPortProxy proxy, string deviceId, CancellationToken token)
    {
        _activeTasks.Add(
            deviceId,
            Validate(
                new ValidatorContext(
                    deviceId,
                    proxy,
                    new MessageReader(proxy)
                ),
                token)
        );
        return Task.CompletedTask;
    }

    private async Task Validate(ValidatorContext context, CancellationToken cancellationToken = default)
    {
        logger.LogInformation("Device Validation for {deviceId} has started.", context.DeviceId);
        List<(Message, MessageDirection)> messages = new();

        Task SendCommand(IPortProxy port, string? deviceId, Message message, CancellationToken ctx = default)
        {
            messages.Add((message, MessageDirection.Outbound));
            return messageHub.SendCommand(port, deviceId, message, ctx);
        }

        context.MessageReader.Initialize();

        var timeoutCheckCts = new CancellationTokenSource();
        var timeoutCts = new CancellationTokenSource();

        bool validated = false;

        IdentifyResponsePayload? identifyResponsePayload = null;
        InfoResponsePayload? infoResponsePayload = null;

        Task.Run(async () =>
        {
            await Task.Delay(15000, timeoutCheckCts.Token);

            if (timeoutCheckCts.IsCancellationRequested)
            {
                return;
            }

            await timeoutCts.CancelAsync();
            if (OnDeviceInvalidated != null)
            {
                await OnDeviceInvalidated(context.DeviceId[^6..], cancellationToken);
            }

            logger.LogInformation("Device Validation for {deviceId} has failed: Timeout", context.DeviceId);
        }, timeoutCheckCts.Token);


        await SendCommand(
            context.Port,
            SystemUtils.DefaultId[^6..].GetHexString(),
            messageHub.CreateMessage(Command.IdentifyRequest),
            cancellationToken);

        try
        {
            while (!timeoutCts.IsCancellationRequested && !validated)
            {
                var message = await context.MessageReader.ReadNextMessage(timeoutCts.Token);
                if (message == null)
                    continue;

                messages.Add((message, MessageDirection.Inbound));

                List<Message> responseMessages = new();
                switch (message.Command)
                {
                    case Command.IdentifyResponse:
                    {
                        identifyResponsePayload = (IdentifyResponsePayload)message.Payload!;
                        break;
                    }
                    case Command.TimeRequest:
                    {
                        responseMessages.Add(messageHub.CreateMessage(
                            Command.TimeResponse,
                            new TimeResponse(DateTimeOffset.Now)
                        ));
                        responseMessages.Add(messageHub.CreateMessage(
                            Command.InfoRequest
                        ));
                        break;
                    }
                    case Command.InfoResponse:
                    {
                        infoResponsePayload = (InfoResponsePayload)message.Payload!;
                        validated = true;
                        break;
                    }
                    default:
                        break;
                }

                if (validated)
                {
                    await timeoutCheckCts.CancelAsync();
                    break;
                }


                if (responseMessages.Count > 0)
                {
                    foreach (var responseMessage in responseMessages)
                    {
                        await SendCommand(
                            context.Port,
                            message.DeviceId,
                            responseMessage,
                            timeoutCts.Token
                        );
                    }
                }
            }

            await timeoutCheckCts.CancelAsync();

            if (OnDeviceValidated != null && validated)
            {
                logger.LogInformation("Device Validation for {deviceId} has succeed.", context.DeviceId);
                await OnDeviceValidated(
                    context.DeviceId,
                    identifyResponsePayload!,
                    infoResponsePayload!,
                    messages,
                    context.MessageReader,
                    context.Port,
                    cancellationToken
                );
            }
        }
        catch (Exception e)
        {
            logger.LogError(e, "A Failure occurred while validating device: {deviceId}", context.DeviceId);
            if (OnDeviceInvalidated != null)
            {
                await OnDeviceInvalidated(context.DeviceId[^6..], cancellationToken);
            }

            await timeoutCts.CancelAsync();
            await timeoutCheckCts.CancelAsync();
        }
        finally
        {
            _activeTasks.Remove(context.DeviceId);
        }
    }


    record ValidatorContext(string DeviceId, IPortProxy Port, MessageReader MessageReader)
    {
        public CancellationTokenSource CancellationTokenSource { get; } = new();

        public CancellationToken CancellationToken => CancellationTokenSource.Token;
    }
}