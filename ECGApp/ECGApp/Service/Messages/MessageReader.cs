using System;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using ECGLib;
using ECGLib.Messages;
using ECGLib.Messages.Extensions;
using ECGLib.Messages.Payloads;
using ECGLib.Service.Messages;

namespace ECGApp.Service.Messages;

public class MessageReader(IPortProxy serialPort) : IMessageReader
{
    private bool _hasCommandReceived;
    private bool _hasSizeReceived;
    private bool _hasDeviceIdReceived;
    private bool _hasTargetDeviceIdReceived;
    private bool _hasTimeReceived;
    private bool _hasRtcTimeReceived;
    private bool _hasPayloadSizeReceived;
    private bool _receiveInProgress;
    private bool _commandMessageReceived;
    private Command _commandReceived = Command.Unkn;
    private uint _sizeReceived;
    private string? _deviceIdReceived;
    private string? _targetDeviceIdReceived;
    private uint _timeReceived;
    private ulong _rtcTimeReceived;
    private uint _payloadSizeReceived;

    private int _messageIndex;
    private byte[]? _payloadBuffer;
    private int _remBytes;

    private byte[]? _buffer;

    public void Initialize()
    {
        serialPort.WriteTimeout = 1000;
        serialPort.ReadTimeout = 1000;
        serialPort.Open();
    }

    public async Task<Message?> ReadNextMessage(CancellationToken ctx = default)
    {
        Exception? exception = null;
        while (!ctx.IsCancellationRequested)
        {
            try
            {
                while (serialPort.BytesToRead > 0 || _remBytes > 0)
                {
                    if (_buffer == null)
                    {
                        _buffer = new byte[serialPort.BytesToRead];
                        _remBytes = await serialPort.BaseStream.ReadAsync(_buffer, ctx);
                    }
                    else if (serialPort.BytesToRead > 0)
                    {
                        var newBuffer = new byte[serialPort.BytesToRead];
                        var newBytesRead = await serialPort.BaseStream.ReadAsync(newBuffer, ctx);
                        var replacementBuffer = new byte[_buffer.Length + newBytesRead];
                        int startingIndex;
                        if (_remBytes > 0)
                        {
                            startingIndex = _buffer.Length;
                            Array.Copy(_buffer, replacementBuffer, _buffer.Length);
                        }
                        else
                        {
                            startingIndex = 0;
                            _messageIndex = 0;
                        }

                        Array.Copy(newBuffer, 0, replacementBuffer, startingIndex, newBytesRead);
                        _remBytes += newBytesRead;
                        _buffer = replacementBuffer;
                    }
                    
                    
                    // Console.WriteLine($"Data: ");
                    // for (int i = 0; i < _buffer.Length; i++)
                    // {
                    //     if (i % 16 == 0 && i != 0)
                    //     {
                    //         Console.WriteLine();
                    //     }
                    //
                    //     Console.Write($"{_buffer[i]:X2} ");
                    // }
                    // Console.WriteLine("-----------");

                    while (_remBytes > 0 && !_commandMessageReceived)
                    {
                        if (!_receiveInProgress)
                        {
                            Clear(false);
                            _receiveInProgress = true;
                        }

                        if (!await ReadMessageHeaders())
                        {
                            break;
                        }
                    }

                    if (_commandMessageReceived && !_receiveInProgress)
                    {
                        return ParseCommand();
                    }
                }
                
                await Task.Delay(1, ctx);
            }
            catch (Exception e)
            {
                exception = e;
                break;
            }
        }

        serialPort.Dispose();
        if (exception is not null)
        {
            Console.WriteLine(exception);
            throw exception;
        }

        return null;
    }

    private Message? ParseCommand()
    {
        if (!_commandMessageReceived)
        {
            return null;
        }

        _commandMessageReceived = false;

        if (!_hasCommandReceived)
        {
            return null;
        }

        try
        {
            var baseMessage = new Message(
                _commandReceived,
                _deviceIdReceived!,
                _targetDeviceIdReceived!,
                _timeReceived,
                DateTimeOffset.FromUnixTimeSeconds((long)_rtcTimeReceived)
            );

            var payloadCtor = IPayload.CommandPayloadConstructors.TryGetValue(_commandReceived, out var ctor)
                ? ctor
                : _ => null;


            return baseMessage with
            {
                Payload = payloadCtor(_payloadBuffer!)
            };
        }
        catch (Exception)
        {
            Clear();
            return null;
        }
    }

    private async Task<bool> ReadMessageHeaders()
    {
        byte[]? byteBuffer;
        if (!_hasCommandReceived)
        {
            if (!ReadSingle(out var rc) || rc == null)
            {
                return false;
            }

            if (rc > (byte)Command.Total || rc == (byte)Command.Unkn)
            {
                Clear();
                return false;
            }

            _commandReceived = (Command)rc;
            _hasCommandReceived = true;
        }
        else if (!_hasSizeReceived)
        {
            if (!Read(sizeof(uint), out byteBuffer) || byteBuffer is null)
            {
                return false;
            }

            _sizeReceived = byteBuffer.FromBytes<uint>();
            if (_sizeReceived < 0 || _sizeReceived > 1024)
            {
                Clear();
                return false;
            }

            _hasSizeReceived = true;
        }
        else if (!_hasDeviceIdReceived)
        {
            if (!Read(6, out byteBuffer) || byteBuffer is null)
            {
                return false;
            }

            _deviceIdReceived = byteBuffer.GetHexString();
            _hasDeviceIdReceived = true;
        }
        else if (!_hasTargetDeviceIdReceived)
        {
            if (!Read(6, out byteBuffer) || byteBuffer is null)
            {
                return false;
            }

            _targetDeviceIdReceived = byteBuffer.GetHexString();
            _hasTargetDeviceIdReceived = true;
        }
        else if (!_hasTimeReceived)
        {
            if (!Read(sizeof(uint), out byteBuffer) || byteBuffer is null)
            {
                return false;
            }

            _timeReceived = byteBuffer.FromBytes<uint>();
            _hasTimeReceived = true;
        }
        else if (!_hasRtcTimeReceived)
        {
            if (!Read(sizeof(ulong), out byteBuffer) || byteBuffer is null)
            {
                return false;
            }

            _rtcTimeReceived = byteBuffer.FromBytes<ulong>();
            _hasRtcTimeReceived = true;
        }
        else if (!_hasPayloadSizeReceived)
        {
            if (!Read(sizeof(uint), out byteBuffer) || byteBuffer is null)
            {
                return false;
            }

            _payloadSizeReceived = byteBuffer.FromBytes<uint>();
            if (_payloadSizeReceived > 1024)
            {
                Clear();
                return false;
            }

            _hasPayloadSizeReceived = true;
            if (_payloadSizeReceived == 0)
            {
                await CompleteConsume();
                return true;
            }
        }
        else if (_hasPayloadSizeReceived)
        {
            if (!Read((int)_payloadSizeReceived, out byteBuffer))
            {
                return false;
            }

            _payloadBuffer = byteBuffer;
            await CompleteConsume();
            return true;
        }

        return true;
    }

    private Task CompleteConsume()
    {
        if (_buffer == null)
        {
            throw new Exception("Invalid State");
        }

        _receiveInProgress = false;
        _commandMessageReceived = true;
        if (_remBytes > 0)
        {
            byte[] newBuffer = new byte[_remBytes];
            Array.Copy(_buffer[_messageIndex..], newBuffer, _remBytes);
            _buffer = newBuffer;
        }
        else
        {
            _buffer = null;
            _remBytes = 0;
        }

        _messageIndex = 0;
        return Task.CompletedTask;
    }

    private bool Read(int size, out byte[]? result)
    {
        if (_buffer == null)
        {
            throw new Exception("Invalid State");
        }

        if (_remBytes < size)
        {
            result = null;
            return false;
        }

        result = new byte[size];
        try
        {
            Array.Copy(_buffer[_messageIndex..], result, size);
        }
        catch (Exception)
        {
            Console.WriteLine(
                $"Failed to read message: R:{_remBytes}, S:{size}, B:{_buffer?.Length ?? 0}, M:{_messageIndex}");
            throw;
        }

        _messageIndex += size;
        _remBytes -= size;
        return true;
    }

    private bool ReadSingle(out byte? result)
    {
        if (_buffer == null)
        {
            throw new Exception("Invalid State");
        }

        if (_remBytes == 0)
        {
            result = null;
            return false;
        }

        result = _buffer[_messageIndex++];
        _remBytes--;
        return true;
    }

    private void Clear(bool fatal = true)
    {
        if (fatal)
        {
            CompleteConsume();
            // Console.WriteLine(Encoding.ASCII.GetString(_buffer ?? []));
            // throw new Exception("Invalid State: Cleared buffers");
        }
        else
        {
            _messageIndex = 0;
        }

        _receiveInProgress = false;
        _commandMessageReceived = false;
        _hasCommandReceived = false;
        _hasDeviceIdReceived = false;
        _hasTargetDeviceIdReceived = false;
        _hasSizeReceived = false;
        _hasRtcTimeReceived = false;
        _hasPayloadSizeReceived = false;
        _hasTimeReceived = false;
        _payloadSizeReceived = 0;
        _commandReceived = Command.Unkn;
        _deviceIdReceived = null;
        _targetDeviceIdReceived = null;
        _rtcTimeReceived = 0;
        _payloadBuffer = null;
    }

    public void Dispose()
    {
        try
        {
            serialPort.Close();
        }
        catch
        {
            // ignored
        }

        serialPort?.Dispose();
    }
}