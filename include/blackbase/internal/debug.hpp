#pragma once

#include <blackbase/internal/system.hpp>
#include <blackbase/internal/exports.hpp>

#if defined(_MSC_VER)
    #define BLACKBASE_DEBUG_BREAK() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
    #define BLACKBASE_DEBUG_BREAK() __builtin_trap()
#else
    #define BLACKBASE_DEBUG_BREAK() ((void)0)
#endif

#ifdef BLACKBASE_SYSTEM_WINDOWS
    extern "C" BLACKBASE_IMPORT int __stdcall IsDebuggerPresent(void);

    #define TRAP_UNDER_DEBUGGER() \
        do { \
            if (IsDebuggerPresent()) \
            { \
                BLACKBASE_DEBUG_BREAK(); \
            } \
        } while (0)
#elif defined(BLACKBASE_SYSTEM_LINUX)
    #include <unistd.h>
    #include <string>
    #include <fstream>

    inline bool isDebuggerAttached()
    {
        std::ifstream statusFile("/proc/self/status");
        if (!statusFile.is_open())
            return false;

        std::string line;
        while (std::getline(statusFile, line))
        {
            if (line.find("TracerPid:") != std::string::npos)
            {
                int tracerPid = std::stoi(line.substr(10));
                return tracerPid != 0;
            }
        }
        return false;
    }

    #define TRAP_UNDER_DEBUGGER() \
        do { \
            if (isDebuggerAttached()) \
            { \
                BLACKBASE_DEBUG_BREAK(); \
            } \
        } while (0)
#else
    #define TRAP_UNDER_DEBUGGER() ((void)0)
#endif