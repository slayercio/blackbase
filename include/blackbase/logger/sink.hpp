#pragma once
#include <blackbase/common.hpp>


namespace blackbase
{
    enum class LogLevel : std::uint8_t
    {
        Trace    = 0,
        Debug    = 1,
        Info     = 2,
        Warning  = 3,
        Error    = 4,
        Critical = 5
    };

    class Sink
    {
    public:
        virtual ~Sink() = default;

        virtual void Log(const std::string& message, LogLevel level) = 0;
        virtual void Stacktrace(const std::string& message, LogLevel level, const std::string& formattedStacktrace) = 0;

        virtual void Flush() = 0;

        virtual const std::string GetName() const = 0;
        virtual LogLevel GetLogLevel() const = 0;
        virtual void SetLogLevel(LogLevel level) = 0;
    };

    BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR std::string GetLevelString(LogLevel level)
    {
        std::string out;

        switch (level)
        {
            case LogLevel::Trace:    out = "TRACE"; break;
            case LogLevel::Debug:    out = "DEBUG"; break;
            case LogLevel::Info:     out = "INFO"; break;
            case LogLevel::Warning:  out = "WARNING"; break;
            case LogLevel::Error:    out = "ERROR"; break;
            case LogLevel::Critical: out = "CRITICAL"; break;
        }

        return out;
    }
}