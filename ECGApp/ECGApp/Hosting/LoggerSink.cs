using System;
using System.Collections.Generic;
using System.Linq;
using Avalonia.Logging;
using Microsoft.Extensions.Logging;

namespace ECGApp.Hosting;

internal class LoggerSink : ILogSink
{
    private readonly ILogger<LoggerSink> logger;
    private IReadOnlyCollection<string> selectedAreas;

    public LoggerSink(ILogger<LoggerSink> logger, params string[] areas)
    {
        this.logger = logger;
        this.selectedAreas = areas ?? Array.Empty<string>();
    }

    bool ILogSink.IsEnabled(LogEventLevel level, string area)
    {
        return this.logger.IsEnabled(FromLogEventLevel(level)) && this.selectedAreas.Contains(area);
    }

    void ILogSink.Log(LogEventLevel level, string area, object? source, string messageTemplate)
    {
        throw new NotImplementedException();
    }

    void ILogSink.Log(LogEventLevel level, string area, object? source, string messageTemplate, params object?[] propertyValues)
    {
        LogLevel concreteLevel = FromLogEventLevel(level);
        if (this.logger.IsEnabled(concreteLevel))
        {
            string eventId = $"AvaloniaHost[{area}]";
            if (source is not null)
            {
                eventId = $"{eventId}+{Convert.ToString(source)}";
            }

            this.logger.Log(concreteLevel, new EventId(1, eventId), messageTemplate, propertyValues);
        }
    }

    private static LogLevel FromLogEventLevel(LogEventLevel eventLevel)
    {
        return eventLevel switch {
            LogEventLevel.Verbose => LogLevel.Trace,
            LogEventLevel.Debug => LogLevel.Debug,
            LogEventLevel.Information => LogLevel.Information,
            LogEventLevel.Warning => LogLevel.Warning,
            LogEventLevel.Error => LogLevel.Error,
            LogEventLevel.Fatal => LogLevel.Critical,
            _ => LogLevel.None,
        };
    }
}