#include <blackbase/logger/logger.hpp>
#include <blackbase/logger/logger_instance.hpp>
#include <blackbase/logger/sinks/file_sink.hpp>

__declspec(noinline) void nested_one()
{
    blackbase::Logger::Info(xorstr_("This is an info message from nested_one."));
    blackbase::Logger::Warning(xorstr_("This is a warning message from nested_one."));
    blackbase::Logger::Error(xorstr_("This is an error message with a stacktrace from nested_one."));
    blackbase::Logger::Critical(xorstr_("This is a critical error message with a stacktrace from nested_one."));
}

__declspec(noinline) void nested_two()
{
    nested_one();
}

__declspec(noinline) void nested_three()
{
    nested_two();
}

__declspec(noinline) void nested_four()
{
    nested_three();
}

__declspec(noinline) void nested_five()
{
    nested_four();
}

__declspec(noinline) void nested_counter(size_t count)
{
    if (count == 0)
    {
        blackbase::Logger::Critical(xorstr_("Reached the base case of nested_counter."));

        return;
    }

    nested_counter(count - 1);
}

int main()
{
    auto logger = blackbase::Logger::Create();
    logger->AddSink(blackbase::FileSink::Create(xorstr_("log.txt")));


    blackbase::Logger::Info(xorstr_("This is an info message."));
    blackbase::Logger::Warning(xorstr_("This is a warning message."));
    blackbase::Logger::Error(xorstr_("This is an error message with a stacktrace."));
    blackbase::Logger::Critical(xorstr_("This is a critical error message with a stacktrace."));

    nested_five();
    nested_counter(20);

    logger->Destroy();
}