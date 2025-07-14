#pragma once
#include <blackbase/util/string.hpp>

namespace blackbase::util
{
    BLACKBASE_API std::wstring ToWide(const std::string_view& str)
    {
        return std::wstring(str.begin(), str.end());
    }

    BLACKBASE_API std::wstring ToWide(const char* str)
    {
        return std::wstring(str, str + std::strlen(str));
    }

    BLACKBASE_API std::wstring ToWide(char* str, size_t size)
    {
        return std::wstring(str, str + size);
    }

    BLACKBASE_API std::wstring ToWide(std::string str)
    {
        return std::wstring(str.begin(), str.end());
    }

    BLACKBASE_API std::string ToNarrow(const std::wstring_view& str)
    {
        return std::string(str.begin(), str.end());
    }

    BLACKBASE_API std::string ToNarrow(const wchar_t* str)
    {
        return std::string(str, str + wcslen(str));
    }

    BLACKBASE_API std::string ToNarrow(wchar_t* str, size_t size)
    {
        return std::string(str, str + size);
    }

    BLACKBASE_API std::string ToNarrow(std::wstring str)
    {
        return std::string(str.begin(), str.end());
    }
}