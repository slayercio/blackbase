#pragma once
#include <string>
#include <string_view>
#include <format>

#define BLACKBASE_CONSTEXPR constexpr
#define BLACKBASE_INLINE inline

#ifdef _MSC_VER
    #define BLACKBASE_FORCEINLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
    #define BLACKBASE_FORCEINLINE __attribute__((always_inline)) inline
#else
    #define BLACKBASE_FORCEINLINE inline
#endif

#define BLACKBASE_NODISCARD [[nodiscard]]
#define BLACKBASE_NOEXCEPT noexcept
#define BLACKBASE_NORETURN [[noreturn]]

template <typename... Args>
BLACKBASE_FORCEINLINE std::string runtime_format(std::string_view fmt, Args&&... args) {
    if constexpr (sizeof...(Args) > 0) {
        return std::vformat(fmt, std::make_format_args(args...));
    } else {
        return std::string(fmt);
    }
}

template<typename... Args>
BLACKBASE_FORCEINLINE std::string runtime_format(char* fmt, Args&&... args) {
    return runtime_format(std::string_view(fmt), std::forward<Args>(args)...);
}

template<typename T, typename... Args>
BLACKBASE_NORETURN void runtime_throw(std::string_view fmt, Args&&... args) {
    throw T(runtime_format(fmt, std::forward<Args>(args)...));
}