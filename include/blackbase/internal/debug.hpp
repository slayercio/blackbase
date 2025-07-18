#pragma once
#include <blackbase/internal/exports.hpp>

#if defined(_MSC_VER)
    #define BLACKBASE_DEBUG_BREAK() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
    #define BLACKBASE_DEBUG_BREAK() __builtin_trap()
#else
    #define BLACKBASE_DEBUG_BREAK() ((void)0)
#endif

extern "C" BLACKBASE_IMPORT int __stdcall IsDebuggerPresent(void);

#define TRAP_UNDER_DEBUGGER() \
    do { \
        if (IsDebuggerPresent()) \
        { \
            BLACKBASE_DEBUG_BREAK(); \
        } \
    } while (0)