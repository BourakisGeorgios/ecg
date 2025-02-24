using System;
using System.Threading;
using System.Threading.Tasks;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.ApplicationLifetimes;
using Microsoft.Extensions.Logging;

namespace ECGApp.Hosting;

internal sealed class ControlledHostedLifetime : HostedLifetimeBase<IControlledApplicationLifetime>
{
    private readonly ILogger<IHostedLifetime> logger;

    internal ControlledHostedLifetime(ILogger<ControlledHostedLifetime> logger,IControlledApplicationLifetime runtime) : base(runtime)
    {
        this.logger = logger;
    }
    
    internal ControlledHostedLifetime(ILogger<IHostedLifetime> logger,IControlledApplicationLifetime runtime) : base(runtime)
    {
        this.logger = logger;
    }

    public override Task<int> StartAsync(Application application, CancellationToken cancellationToken)
    {
        int RunInControlledBackground()
        {
            try 
            {
                application.Run(cancellationToken);
                return ExitCodes.ExitSuccessfully;
            }
            catch (Exception ex)
            {
                if (this.logger.IsEnabled(LogLevel.Critical))
                {
                    this.logger.LogCritical("Failure while running application", ex);
                }
                
                return ExitCodes.ExitWithError;
            }
        }

        return Task.Run(RunInControlledBackground, cancellationToken);
    }

    public override Task StopAsync(Application application, CancellationToken cancellationToken)
    {
        void ShutdownWithExitCodeZero()
        {
            this.Runtime.Shutdown(ExitCodes.ExitSuccessfully);
        }

        return Task.Run(ShutdownWithExitCodeZero, cancellationToken);
    }
}