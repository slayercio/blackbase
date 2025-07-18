#pragma once

#ifdef NDEBUG
    #define BLACKBASE_ASSERT(condition, message, ...) ((void)0)
    #define BLACKBASE_DEBUG_BREAK() ((void)0)
#else
    #include <format>
    #include <blackbase/internal/debug.hpp>

    #define BLACKBASE_FORMAT_ASSERT_MSG(condition, message, ...)                \
        std::format(                                                            \
            "[BLACKBASE ASSERTION FAILED]\n"                                    \
            "\tCondition: {}\n"                                                 \
            "\tMessage: {}\n"                                                   \
            "\tFile: {}\n"                                                      \
            "\tLine: {}",                                                       \
            #condition, std::format(message, __VA_ARGS__), __FILE__, __LINE__   \
        )

    #ifdef BLACKBASE_ASSERTS_THROW
        #include <stdexcept>
        #define BLACKBASE_ASSERT(condition, message, ...)   \
            do {                                            \
                if (!(condition)) {                         \
                    auto msg = BLACKBASE_FORMAT_ASSERT_MSG( \
                        condition, message, __VA_ARGS__);   \
                    TRAP_UNDER_DEBUGGER();                  \
                    throw std::runtime_error(msg);          \
                }                                           \
            } while (false)
    #else
        #include <iostream>
        #define BLACKBASE_ASSERT(condition, message, ...)              \
            do {                                                       \
                if (!(condition)) {                                    \
                    std::cerr << BLACKBASE_FORMAT_ASSERT_MSG(          \
                        condition, message, __VA_ARGS__) << std::endl; \
                    TRAP_UNDER_DEBUGGER();                             \
                    std::abort();                                      \
                }                                                      \
            } while (false)
    #endif // BLACKBASE_ASSERT_THROW
#endif