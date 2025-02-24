using System;
using Avalonia.Controls.ApplicationLifetimes;
using Microsoft.Extensions.Logging;

namespace ECGApp.Hosting;

internal static class HostedLifetime
{
    internal static IHostedLifetime Select(ILoggerFactory loggerFactory, IApplicationLifetime? lifetime)
    {
        return lifetime switch {
            IClassicDesktopStyleApplicationLifetime desktop => new DesktopHostedLifetime(loggerFactory.CreateLogger<DesktopHostedLifetime>(), desktop),
            IControlledApplicationLifetime controlled => new ControlledHostedLifetime(loggerFactory.CreateLogger<ControlledHostedLifetime>(), controlled),
            ISingleViewApplicationLifetime _ => throw new PlatformNotSupportedException("This is only supposed to run on windows, linux and macos. Mobile lifetimes are not supported."),
            _ => new FallbackHostedLifetime(loggerFactory.CreateLogger<FallbackHostedLifetime>()),
        };
    }
}