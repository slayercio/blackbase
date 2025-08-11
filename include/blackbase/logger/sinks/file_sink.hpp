#pragma once
#include <blackbase/common.hpp>
#include <blackbase/logger/sink.hpp>
#include <fstream>

namespace blackbase
{
    class FileSink : public Sink
    {
    private:
        std::ofstream fileStream;
        std::string   filePath;
        LogLevel      logLevel;

    public:
        static std::shared_ptr<FileSink> Create(const std::string& path, LogLevel level = LogLevel::Info);
    
    public:
        ~FileSink() override;

    public:
        void Log(const std::string& message, LogLevel level) override;

    public:
        void Stacktrace(const std::string& message, LogLevel level, const std::string& formattedStacktrace) override;

    public:
        void Flush() override;

    public:
        const std::string GetName() const override;
        LogLevel GetLogLevel() const override;
        void SetLogLevel(LogLevel level) override;

    protected:
        FileSink(const std::string& path, LogLevel level = LogLevel::Info);
    };
}

#pragma region FileSink Implementation
namespace blackbase
{
    std::shared_ptr<FileSink> FileSink::Create(const std::string& path, LogLevel level)
    {
        return std::shared_ptr<FileSink>(new FileSink(path, level));
    }

    FileSink::~FileSink()
    {
        if (fileStream.is_open())
        {
            fileStream.close();
        }
    }

    void FileSink::Log(const std::string& message, LogLevel level)
    {
        if (level >= logLevel && fileStream.is_open())
        {
            fileStream << runtime_format(xorstr_("[{}] {}"), GetLevelString(level), message) << std::endl;
        }
    }

    void FileSink::Stacktrace(const std::string& message, LogLevel level, const std::string& formattedStacktrace)
    {
        if (level >= logLevel && fileStream.is_open())
        {
            fileStream << runtime_format(xorstr_("[{}] {}"), GetLevelString(level), message) << "\n" << formattedStacktrace << std::endl;
        }
    }

    void FileSink::Flush()
    {
        if (fileStream.is_open())
        {
            fileStream.flush();
        }
    }

    const std::string FileSink::GetName() const
    {    
        return runtime_format(xorstr_("FileSink<'{}'>"), filePath);
    }

    LogLevel FileSink::GetLogLevel() const
    {
        return logLevel;
    }

    void FileSink::SetLogLevel(LogLevel level)
    {
        logLevel = level;
    }

    FileSink::FileSink(const std::string& path, LogLevel level)
        : filePath(path), logLevel(level)
    {
        fileStream.open(filePath, std::ios_base::app);
        if (!fileStream.is_open())
        {
            runtime_throw<std::runtime_error>(xorstr_("Failed to open log file: {}"), filePath);
        }
    }
}
#pragma endregion