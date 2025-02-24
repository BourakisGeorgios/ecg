using System;
using System.Threading;
using System.Threading.Tasks;
using Avalonia;
using Avalonia.Controls;
using Microsoft.Extensions.Logging;

namespace ECGApp.Hosting;

internal sealed class FallbackHostedLifetime : IHostedLifetime
{
    private readonly ILogger<FallbackHostedLifetime> logger;

    internal FallbackHostedLifetime(ILogger<FallbackHostedLifetime> logger)
    {
        this.logger = logger;
    }

    public Task<int> StartAsync(Application application, CancellationToken cancellationToken)
    {
        int RunWithCancellationToken()
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
        return Task.Run(RunWithCancellationToken, cancellationToken);
    }

    public Task StopAsync(Application application, CancellationToken cancellationToken)
    {
        return Task.FromException<NotSupportedException>(new NotSupportedException());
    }
}