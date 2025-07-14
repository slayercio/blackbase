#pragma once
#include <blackbase/common.hpp>
#include <string>

namespace blackbase::util
{
    BLACKBASE_API std::wstring ToWide(const std::string_view& str);
    BLACKBASE_API std::wstring ToWide(const char* str);
    BLACKBASE_API std::wstring ToWide(char* str, size_t size);
    BLACKBASE_API std::wstring ToWide(std::string str);

    BLACKBASE_API std::string ToNarrow(const std::wstring_view& str);
    BLACKBASE_API std::string ToNarrow(const wchar_t* str);
    BLACKBASE_API std::string ToNarrow(wchar_t* str, size_t size);
    BLACKBASE_API std::string ToNarrow(std::wstring str);
}