#pragma once
#include <blackbase/internal/exports.hpp>

#define BLACKBASE_INLINE inline

#ifndef BLACKBASE_HEADER_ONLY
    #define BLACKBASE_IMPL    
    #ifndef BLACKBASE_BUILD
        #define BLACKBASE_API BLACKBASE_IMPL BLACKBASE_IMPORT
    #else
        #define BLACKBASE_API BLACKBASE_IMPL BLACKBASE_EXPORT
    #endif
    
#else
    #define BLACKBASE_IMPL BLACKBASE_INLINE
    #define BLACKBASE_API BLACKBASE_IMPL
#endif