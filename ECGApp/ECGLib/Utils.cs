namespace ECGLib;

public sealed class Utils
{
    private static readonly Random Random = new Random(Environment.TickCount);

    public static string GetRandomHexNumber(int digits)
    {
        byte[] buffer = new byte[digits / 2];
        Random.NextBytes(buffer);
        string result = String.Concat(buffer.Select(x => x.ToString("X2")).ToArray());
        if (digits % 2 == 0)
            return result;
        return result + Random.Next(16).ToString("X");
    }
}