#pragma once

#ifdef _MSC_VER
    #define BLACKBASE_FORCEINLINE __forceinline
#else
    #define BLACKBASE_FORCEINLINE inline __attribute__((always_inline))
#endif

