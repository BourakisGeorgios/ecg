namespace ECGLib;

public interface IPortProxy : IDisposable
{
    int WriteTimeout { get; set; }
    int ReadTimeout { get; set; }
    int BytesToRead { get; }
    Stream BaseStream { get; }
    
    String PortName { get; }
    void Open();
    void Close();
}