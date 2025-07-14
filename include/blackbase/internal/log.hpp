#pragma once
#include <format>
#include <source_location>

#ifdef BLACKBASE_ERRORS_THROW
    #include <stdexcept>

    #define BLACKBASE_ERROR(message, ...) \
        throw std::runtime_error(std::format("\033[31m[BLACKBASE ERROR]\033[0m " message, __VA_ARGS__))

    #define BLACKBASE_TRACE(message, ...)                                                                        \
        do {                                                                                                     \
            [&] (std::source_location source = std::source_location::current()) {                                \
                throw std::runtime_error(std::format(                                                            \
                    "\033[34m[BLACKBASE TRACE]\033[0m\n"                                                         \
                    "\tFile: {}\n"                                                                               \
                    "\tLine: {}\n"                                                                               \
                    "\tFunction: {}\n"                                                                           \
                    "\tMessage: {}\n",                                                                           \
                    source.file_name(), source.line(), source.function_name(), std::format(message, __VA_ARGS__) \
                ));                                                                                              \
            }();                                                                                                 \
        } while (0)
#else
    #include <iostream>

    #define BLACKBASE_ERROR(message, ...) \
        std::cerr << std::format("\033[31m[BLACKBASE ERROR]\033[0m " message "\n", __VA_ARGS__)

    #define BLACKBASE_TRACE(message, ...)                                                                        \
        do {                                                                                                     \
            [&] (std::source_location source = std::source_location::current()) {                                \
                std::cerr << std::format(                                                                        \
                    "\033[34m[BLACKBASE TRACE]\033[0m\n"                                                         \
                    "\tFile: {}\n"                                                                               \
                    "\tLine: {}\n"                                                                               \
                    "\tFunction: {}\n"                                                                           \
                    "\tMessage: {}\n",                                                                           \
                    source.file_name(), source.line(), source.function_name(), std::format(message, __VA_ARGS__) \
                );                                                                                               \
                std::terminate();                                                                                \
            }();                                                                                                 \
        } while (0)
#endif

#ifdef NDEBUG
    #define BLACKBASE_DEBUG(message, ...) ((void)0)
#else
    #include <format>
    #include <iostream>

    #define BLACKBASE_DEBUG(message, ...) \
        std::cout << std::format("\033[32m[BLACKBASE DEBUG]\033[0m " message "\n", __VA_ARGS__)
#endif