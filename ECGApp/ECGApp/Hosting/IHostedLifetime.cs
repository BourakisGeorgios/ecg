using System.Threading;
using System.Threading.Tasks;

namespace ECGApp.Hosting;

public interface IHostedLifetime 
{
    Task<int> StartAsync(Avalonia.Application application, CancellationToken cancellationToken);

    Task StopAsync(Avalonia.Application application, CancellationToken cancellationToken);
}