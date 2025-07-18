#pragma once

#ifndef BLACKBASE_HEADER_ONLY
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
#else
    #define BLACKBASE_EXPORT
    #define BLACKBASE_IMPORT
#endif