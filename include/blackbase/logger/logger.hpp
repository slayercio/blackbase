#pragma once
#include <blackbase/common.hpp>
#include <blackbase/logger/sink.hpp>
#include <stacktrace>
#include <sstream>
#include <Windows.h>
#include <winnt.h>
#include <DbgHelp.h>

namespace blackbase
{
    class Logger
    {
    private:
        std::vector<std::shared_ptr<Sink>> m_Sinks;
        LogLevel m_LogLevel = LogLevel::Info;

    public:
        static std::shared_ptr<Logger> Create();
        static void Destroy();

        static std::shared_ptr<Logger> GetInstance();

    public:
        BLACKBASE_FORCEINLINE void AddSink(std::shared_ptr<Sink> sink);
        BLACKBASE_FORCEINLINE void RemoveSink(const std::string& name);

    public:
        template<LogLevel level = LogLevel::Info, typename ...Args>
        BLACKBASE_FORCEINLINE static void Log(const std::string_view& fmt, Args&&... args)
        {
            if (auto logger = GetInstance())
            {
                logger->LogInternal(fmt, std::make_format_args(std::forward<Args>(args)...), level);
            }
        }

        template<LogLevel level = LogLevel::Info, typename ...Args>
        BLACKBASE_FORCEINLINE static void Stacktrace(const std::string_view& fmt, Args&&... args)
        {
            if (auto logger = GetInstance())
            {
                logger->StacktraceInternal(fmt, std::make_format_args(std::forward<Args>(args)...), level);
            }
        }

        template<typename ...Args>
        BLACKBASE_FORCEINLINE static void Trace(const std::string_view& fmt, Args&&... args)
        {
            Stacktrace<LogLevel::Trace, Args...>(fmt, std::forward<Args>(args)...);
        }

        template<typename ...Args>
        BLACKBASE_FORCEINLINE static void Debug(const std::string_view& fmt, Args&&... args)
        {
            Log<LogLevel::Debug, Args...>(fmt, std::forward<Args>(args)...);
        }

        template<typename ...Args>
        BLACKBASE_FORCEINLINE static void Info(const std::string_view& fmt, Args&&... args)
        {
            Log<LogLevel::Info, Args...>(fmt, std::forward<Args>(args)...);
        }

        template<typename ...Args>
        BLACKBASE_FORCEINLINE static void Warning(const std::string_view& fmt, Args&&... args)
        {
            Log<LogLevel::Warning, Args...>(fmt, std::forward<Args>(args)...);
        }

        template<typename ...Args>
        BLACKBASE_FORCEINLINE static void Error(const std::string_view& fmt, Args&&... args)
        {
            Stacktrace<LogLevel::Error, Args...>(fmt, std::forward<Args>(args)...);
        }

        template<typename ...Args>
        BLACKBASE_FORCEINLINE static void Critical(const std::string_view& fmt, Args&&... args)
        {
            Stacktrace<LogLevel::Critical, Args...>(fmt, std::forward<Args>(args)...);
        }

        BLACKBASE_FORCEINLINE void Flush();

    public:
        BLACKBASE_FORCEINLINE const std::vector<std::shared_ptr<Sink>>& GetSinks() const;
        BLACKBASE_FORCEINLINE LogLevel GetLogLevel() const;

        /**
         * Sets the log level for the logger, however each sink may have its own log level.
         */
        BLACKBASE_FORCEINLINE void SetLogLevel(LogLevel level);

    protected:
        BLACKBASE_FORCEINLINE void LogInternal(const std::string& message, LogLevel level);
        BLACKBASE_FORCEINLINE void LogInternal(const std::string_view& fmt, std::format_args args, LogLevel level);

        BLACKBASE_FORCEINLINE void StacktraceInternal(const std::string& message, LogLevel level, std::stacktrace = std::stacktrace::current());
        BLACKBASE_FORCEINLINE void StacktraceInternal(const std::string_view& fmt, std::format_args args, LogLevel level, std::stacktrace = std::stacktrace::current());

    private:
        BLACKBASE_FORCEINLINE std::string FormatStacktrace(const std::stacktrace& stacktrace) const;
    };
}

#pragma region Logger inline implementations
namespace blackbase
{
    void Logger::AddSink(std::shared_ptr<Sink> sink)
    {
        m_Sinks.push_back(std::move(sink));
    }

    void Logger::RemoveSink(const std::string& name)
    {
        auto it = std::remove_if(m_Sinks.begin(), m_Sinks.end(),
            [&name](const std::shared_ptr<Sink>& sink) { return sink->GetName() == name; });

        if (it != m_Sinks.end())
        {
            m_Sinks.erase(it, m_Sinks.end());
        }
    }

    const std::vector<std::shared_ptr<Sink>>& Logger::GetSinks() const
    {
        return m_Sinks;
    }

    LogLevel Logger::GetLogLevel() const
    {
        return m_LogLevel;
    }

    void Logger::SetLogLevel(LogLevel level)
    {
        m_LogLevel = level;
        for (const auto& sink : m_Sinks)
        {
            sink->SetLogLevel(level);
        }
    }

    void Logger::LogInternal(const std::string& message, LogLevel level)
    {
        for (const auto& sink : m_Sinks)
        {
            sink->Log(message, level);
        }
    }

    void Logger::LogInternal(const std::string_view& fmt, std::format_args args, LogLevel level)
    {
        for (const auto& sink : m_Sinks)
        {
            sink->Log(std::vformat(fmt, args), level);
        }
    }

    void Logger::StacktraceInternal(const std::string& message, LogLevel level, std::stacktrace stacktrace)
    {
        std::string formattedStacktrace = FormatStacktrace(stacktrace);
        for (const auto& sink : m_Sinks)
        {
            sink->Stacktrace(message, level, formattedStacktrace);
        }
    }

    void Logger::StacktraceInternal(const std::string_view& fmt, std::format_args args, LogLevel level, std::stacktrace stacktrace)
    {
        std::string formattedMessage = std::vformat(fmt, args);
        std::string formattedStacktrace = FormatStacktrace(stacktrace);
        for (const auto& sink : m_Sinks)
        {
            sink->Stacktrace(formattedMessage, level, formattedStacktrace);
        }
    }

    void Logger::Flush()
    {
        for (const auto& sink : m_Sinks)
        {
            sink->Flush();
        }
    }

    
    std::string Logger::FormatStacktrace(const std::stacktrace& stacktrace) const
    {
    #ifdef USE_STD_STACKTRACE
        std::ostringstream oss;
        for (size_t i = 0; i < stacktrace.size(); ++i)
        {
            const auto& frame = stacktrace[i];

            oss << runtime_format(xorstr_("#{:02} "), i);

            oss << frame.description();

            if (!frame.source_file().empty())
            {
                oss << runtime_format(xorstr_(" ({}:{})"), frame.source_file(), frame.source_line());
            }

            oss << xorstr_("\n");
        }

        return oss.str();
    #else
        #pragma comment(lib, "dbghelp.lib")

        void* frames[64];
        const auto frameCount = RtlCaptureStackBackTrace(0, 64, frames, nullptr);

        HANDLE process = GetCurrentProcess();

        if (!SymInitialize(process, nullptr, TRUE))
        {
            return xorstr_("Failed to initialize symbol handler.");
        }

        SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);

        std::ostringstream oss;
        oss << runtime_format(xorstr_("Stacktrace ({}) frames:\n"), frameCount);

        for (auto i = 0; i < frameCount; ++i)
        {
            auto address = frames[i];

            char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(CHAR)];
            SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(symbolBuffer);
            symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            symbol->MaxNameLen = MAX_SYM_NAME;

            DWORD64 displacement = 0;
            if (SymFromAddr(process, reinterpret_cast<DWORD64>(address), &displacement, symbol))
            {
                oss << runtime_format(xorstr_("#{} {}"), i, symbol->Name);

                if (displacement != 0)
                {
                    oss << runtime_format(xorstr_("+0x{:x}"), displacement);
                }
            }
            else
            {
                oss << runtime_format(xorstr_("#{} <unknown>"), i);
            }

            IMAGEHLP_LINE64 line = {};
            line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
            DWORD lineDisplacement = 0;
            if (SymGetLineFromAddr64(process, reinterpret_cast<DWORD64>(address), &lineDisplacement, &line))
            {
                oss << runtime_format(xorstr_(" at {}:{}"), line.FileName, line.LineNumber);
            }

            oss << xorstr_("\n");
        }

        SymCleanup(process);

        return oss.str();
    #endif
    }

    extern std::shared_ptr<Logger> g_Logger;
    BLACKBASE_FORCEINLINE std::shared_ptr<Logger> Logger::GetInstance()
    {
        return g_Logger;
    }
}
#pragma endregion