#pragma once
#include <blackbase/internal/system.hpp>

#ifndef BLACKBASE_HEADER_ONLY
    #if defined(BLACKBASE_SYSTEM_WINDOWS)
        #if defined(_MSC_VER)
            #define BLACKBASE_EXPORT __declspec(dllexport)
            #define BLACKBASE_IMPORT __declspec(dllimport)
        #elif defined(__GNUC__)
            #define BLACKBASE_EXPORT __attribute__((dllexport))
            #define BLACKBASE_IMPORT __attribute__((dllimport))
        #else
            #define BLACKBASE_EXPORT
            #define BLACKBASE_IMPORT
        #endif
    #elif defined(BLACKBASE_SYSTEM_LINUX) || defined(BLACKBASE_SYSTEM_MACOS)
        #if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
            #define BLACKBASE_EXPORT __attribute__((visibility("default")))
            #define BLACKBASE_IMPORT
        #else
            #define BLACKBASE_EXPORT
            #define BLACKBASE_IMPORT
        #endif
    #else
        #define BLACKBASE_EXPORT
        #define BLACKBASE_IMPORT
    #endif
#else
    #define BLACKBASE_EXPORT
    #define BLACKBASE_IMPORT
#endif