using System;
using System.ComponentModel.Design;
using System.Threading.Tasks;
using Avalonia;
using ECGApp.Hosting.Extensions;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Projektanker.Icons.Avalonia;
using Projektanker.Icons.Avalonia.FontAwesome;
using Projektanker.Icons.Avalonia.MaterialDesign;

namespace ECGApp;

sealed class Program
{
    // Initialization code. Don't use any Avalonia, third-party APIs or any
    // SynchronizationContext-reliant code before AppMain is called: things aren't initialized
    // yet and stuff might break.
    [STAThread]
    public static async Task<int> Main(string[] args) => await Host
        .CreateDefaultBuilder(args)
        .ConfigureHostOptions(x => x.ShutdownTimeout = TimeSpan.FromSeconds(5))
        .ConfigureServices(s => s
            .AddDependencies()
            .AttachLoggerToAvaloniaLogger())
        .ConfigureAvaloniaAppBuilder<App>(
            BuildAvaloniaAppConfigured,
            a => a.SetupWithClassicDesktopLifetime(args))
        .Build()
        .RunAvaloniaAppAsync();

    // Avalonia configuration, don't remove; also used by visual designer.
    public static AppBuilder BuildAvaloniaAppConfigured(IServiceProvider serviceProvider)
    {
        IconProvider.Current
            .Register<FontAwesomeIconProvider>()
            .Register<MaterialDesignIconProvider>();

        return AppBuilder.Configure<App>(() => new App(serviceProvider))
            .UsePlatformDetect()
            .WithInterFont()
            .LogToTrace();
    }  
    
    // Avalonia configuration, don't remove; also used by visual designer.
    public static AppBuilder BuildAvaloniaApp()
    {
        return BuildAvaloniaAppConfigured(new ServiceCollection()
            .AddDependencies(false)
            .AttachLoggerToAvaloniaLogger()
            .BuildServiceProvider());
    }
}