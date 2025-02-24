using ECGApp.Service;
using ECGApp.Service.Messages;
using ECGApp.ViewModels;
using Microsoft.Extensions.DependencyInjection;

namespace ECGApp;

public static class Dependencies
{
    public static IServiceCollection AddDependencies(this IServiceCollection collection, 
                                                     bool addHostedService = true)
    {
        collection = collection
            .AddSingleton<DeviceManager>()
            .AddSingleton<DeviceDiscoverer>()
            .AddSingleton<MessageHub>()
            .AddSingleton<DeviceValidator>()
            .AddSingleton<MainWindowViewModel>()
            .AddLogging();

        if (addHostedService)
        {
            collection
                .AddHostedService<DeviceManager>();
        }

        return collection;

    }
}