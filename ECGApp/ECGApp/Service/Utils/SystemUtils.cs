using System;
using System.Linq;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using DeviceId;
using ECGLib.Messages;

namespace ECGMonitor.Device.Utils;

public static class SystemUtils
{
    public static readonly int CommandPadding = Enum.GetValues<Command>()
        .Select(x => x.ToString().Length)
        .Max();

    public static readonly int DeviceTypePadding = Enum.GetValues<DeviceType>()
        .Select(x => x.ToString().Length)
        .Max();

    private static byte[]? _hostDeviceId;

    public static byte[] HostDeviceId
    {
        get
        {
            if (_hostDeviceId is null)
            {
                var deviceId = new DeviceIdBuilder()
                    .OnWindows(windows => windows
                        .AddMachineGuid()
                        .AddProcessorId()
                        .AddMotherboardSerialNumber())
                    .ToString();
                using var hashAlgorithm = new HMACSHA1();
                _hostDeviceId = hashAlgorithm.ComputeHash(Encoding.Default.GetBytes(deviceId));
            }

            return _hostDeviceId;
        }
    }

    public static readonly byte[] DefaultId = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];

    [DllImport("kernel32")]
    public static extern UInt64 GetTickCount64();
}