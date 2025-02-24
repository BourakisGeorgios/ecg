using System.IO.Ports;

namespace ECGLib;

public class SerialPortProxy(SerialPort serialPort) : IPortProxy
{
    public void Dispose() => serialPort.Dispose();

    public int WriteTimeout
    {
        get => serialPort.WriteTimeout;
        set => serialPort.WriteTimeout = value;
    }

    public int ReadTimeout
    {
        get => serialPort.ReadTimeout;
        set => serialPort.ReadTimeout = value;
    }

    public int BytesToRead => serialPort.BytesToRead;
    
    public String PortName => serialPort.PortName;

    public Stream BaseStream => serialPort.BaseStream;
    public void Open() => serialPort.Open();
    public void Close() => serialPort.Close();
}