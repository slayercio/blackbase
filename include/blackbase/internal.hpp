#pragma once
#include <string>
#include <string_view>
#include <format>

template <typename... Args>
inline std::string runtime_format(std::string_view fmt, Args&&... args) {
    if constexpr (sizeof...(Args) > 0) {
        return std::vformat(fmt, std::make_format_args(args...));
    } else {
        return std::string(fmt);
    }
}

template<typename... Args>
inline std::string runtime_format(char* fmt, Args&&... args) {
    return runtime_format(std::string_view(fmt), std::forward<Args>(args)...);
}

template<typename T, typename... Args>
[[noreturn]] void runtime_throw(std::string_view fmt, Args&&... args) {
    throw T(runtime_format(fmt, std::forward<Args>(args)...));
}