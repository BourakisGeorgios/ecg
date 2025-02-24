using System.Threading;
using System.Threading.Tasks;
using Avalonia;
using IApplicationLifetime = Avalonia.Controls.ApplicationLifetimes.IApplicationLifetime;

namespace ECGApp.Hosting;

internal abstract class HostedLifetimeBase<TRuntime>: IHostedLifetime where TRuntime: IApplicationLifetime  
{
    public int ExitCode { get; } = 0;
    
    protected HostedLifetimeBase(TRuntime runtime)
    {
        this.Runtime = runtime;
    }

    protected TRuntime Runtime { get; private init; }

    public abstract Task<int> StartAsync(Application application, CancellationToken cancellationToken);
    public abstract Task StopAsync(Application application, CancellationToken cancellationToken);
}