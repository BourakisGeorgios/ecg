using System;
using System.Runtime.Versioning;
using System.Threading;
using System.Threading.Tasks;
using Avalonia;
using Avalonia.Logging;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;

namespace ECGApp.Hosting.Extensions;

public static class AvaloniaHostingExtensions
{
    public static IHostBuilder ConfigureAvalonia<TApplication>(this IHostBuilder hostBuilder,
                                                               Action<AppBuilder> configureAppBuilder,
                                                               IHostedLifetime? lifetime = null)
        where TApplication : Application, new()
    {
        return hostBuilder.ConfigureAvalonia((s) => new TApplication(), configureAppBuilder, lifetime);
    }

    public static IHostBuilder ConfigureAvalonia<TApplication>(this IHostBuilder hostBuilder,
                                                               Func<IServiceProvider, TApplication> applicationResolver,
                                                               Action<AppBuilder> configureAppBuilder,
                                                               IHostedLifetime? lifetime = null)
        where TApplication : Application
    {
        return hostBuilder.ConfigureAvaloniaAppBuilder<TApplication>(
            (s) => AppBuilder.Configure(() => applicationResolver(s)),
            configureAppBuilder, lifetime);
    }

    public static IHostBuilder ConfigureAvaloniaAppBuilder<TApplication>(
        this IHostBuilder hostBuilder,
        Func<IServiceProvider, AppBuilder> appBuilderResolver,
        Action<AppBuilder> configureAppBuilder,
        IHostedLifetime? lifetime = null) where TApplication : Application
    {
        ArgumentNullException.ThrowIfNull(configureAppBuilder);

        hostBuilder.ConfigureServices((ctx, s) =>
        {
            s.AddSingleton<AppBuilder>(provider =>
            {
                AppBuilder appBuilder = appBuilderResolver(provider);
                configureAppBuilder(appBuilder);
                if (appBuilder.Instance is null)
                {
                    appBuilder.SetupWithoutStarting();
                }

                return appBuilder;
            });

            s.AddSingleton<Application>(provider => provider.GetRequiredService<AppBuilder>().Instance!);
            s.AddSingleton<TApplication>(provider =>
                (TApplication)provider.GetRequiredService<AppBuilder>().Instance!);
            s.AddSingleton<IHostedLifetime>(p =>
                lifetime ?? HostedLifetime.Select(p.GetRequiredService<ILoggerFactory>(),
                    p.GetRequiredService<Application>().ApplicationLifetime));
        });

        return hostBuilder;
    }

    [SupportedOSPlatform("windows")]
    [SupportedOSPlatform("linux")]
    public static async Task<int> RunAvaloniaAppAsync(this IHost host, CancellationToken token = default)
    {
        var cancellationTokenSource = CancellationTokenSource.CreateLinkedTokenSource(token);
        var cancellationToken = cancellationTokenSource.Token;
        IHostedLifetime lifetime = host.Services.GetRequiredService<IHostedLifetime>();
        Application application = host.Services.GetRequiredService<Application>();
        await host.StartAsync(cancellationToken);
        int result = await lifetime.StartAsync(application, cancellationToken);

        Environment.Exit(result);
        await cancellationTokenSource.CancelAsync();
        await host.StopAsync(TimeSpan.FromSeconds(5));


        return result;
    }


    public static IServiceCollection AttachLoggerToAvaloniaLogger(this IServiceCollection services,
                                                                  params string[] areas)
    {
        services.AddSingleton<ILogSink>(p => new LoggerSink(p.GetRequiredService<ILogger<LoggerSink>>(), areas));

        return services;
    }
}