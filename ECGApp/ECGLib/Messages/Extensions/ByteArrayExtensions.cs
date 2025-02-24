using System.Runtime.InteropServices;
using System.Text;

namespace ECGLib.Messages.Extensions;

public static class ByteArrayExtensions
{
    public static byte ToByte(this bool boolean)
    {
        return (byte)(boolean ? 0x1 : 0x0);
    }

    public static bool ToBool(this byte value)
    {
        return value == 0x1;
    }

    public static String GetHexString(this byte[] array)
    {
        var sb = new StringBuilder();
        foreach (var b in array)
        {
            sb.Append($"{b:X2}");
        }

        return sb.ToString();
    }

    public static T FromBytes<T>(this byte[] arr) where T : struct
    {
        // if (BitConverter.IsLittleEndian)
        // {
        //     Array.Reverse(arr);
        // }

        T obj;
        GCHandle h = default(GCHandle);
        try
        {
            h = GCHandle.Alloc(arr, GCHandleType.Pinned);
            obj = Marshal.PtrToStructure<T>(h.AddrOfPinnedObject());
        }
        finally
        {
            if (h.IsAllocated)
            {
                h.Free();
            }
        }

        return obj;
    }
}