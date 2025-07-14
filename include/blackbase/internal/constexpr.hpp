#pragma once

#ifdef BLACKBASE_HEADER_ONLY
    #define BLACKBASE_CONSTEXPR constexpr
#else
    #define BLACKBASE_CONSTEXPR
#endif