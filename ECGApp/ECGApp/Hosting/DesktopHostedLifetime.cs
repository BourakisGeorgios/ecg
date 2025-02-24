using System;
using System.Threading;
using System.Threading.Tasks;
using Avalonia.Controls;
using Avalonia.Controls.ApplicationLifetimes;
using Microsoft.Extensions.Logging;

namespace ECGApp.Hosting;

internal sealed class DesktopHostedLifetime : HostedLifetimeBase<IClassicDesktopStyleApplicationLifetime>
{
    private readonly ILogger<DesktopHostedLifetime> logger;

    internal DesktopHostedLifetime(ILogger<DesktopHostedLifetime> logger, IClassicDesktopStyleApplicationLifetime runtime) : base(runtime)
    {
        this.logger = logger;
    }

    public override async Task<int> StartAsync(Avalonia.Application application, CancellationToken cancellationToken)
    {
        if (this.Runtime is ClassicDesktopStyleApplicationLifetime lifetime)
        {
            int result;
            try 
            {
                result = lifetime.Start(lifetime.Args ?? Array.Empty<string>());
            }
            catch (Exception ex)
            {
                if (this.logger.IsEnabled(LogLevel.Critical))
                {
                    this.logger.LogCritical("Failure while running application", ex);
                }

                result = ExitCodes.ExitWithError;
            }
             
            return await Task.FromResult(result);
        }
        
        return await new ControlledHostedLifetime(this.logger, this.Runtime).StartAsync(application, cancellationToken);
    }

    public override async Task StopAsync(Avalonia.Application application, CancellationToken cancellationToken)
    {
        Window? mainWindow = this.Runtime.MainWindow;
        if (mainWindow is not null)
        {
            switch (this.Runtime.ShutdownMode)
            {
                case ShutdownMode.OnMainWindowClose:
                    await Task.Run(mainWindow.Close, cancellationToken);
                    return;
                case ShutdownMode.OnLastWindowClose:
                    foreach (Window window in this.Runtime.Windows)
                    {
                        if (!ReferenceEquals(mainWindow, window))
                        {
                            await Task.Run(window.Close, cancellationToken);
                        }
                    }

                    await Task.Run(mainWindow.Close, cancellationToken);
                    return;
                case ShutdownMode.OnExplicitShutdown:
                    await Task.Run(() => this.Runtime.Shutdown(), cancellationToken);
                    return;
            }
        }

        await new ControlledHostedLifetime(this.logger, this.Runtime).StopAsync(application, cancellationToken);
    }
}