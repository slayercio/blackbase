#pragma once
#include <blackbase/common.hpp>
#include <Windows.h>
#include <stringapiset.h>

namespace blackbase
{
    inline std::wstring ToWideString(const std::string_view& str)
    {
        if (str.empty())
            return std::wstring();

        int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), nullptr, 0);
        std::wstring wideStr(size_needed, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), wideStr.data(), size_needed);
        return wideStr;
    }

    inline std::string ToString(const std::wstring_view& wstr)
    {
        if (wstr.empty())
            return std::string();

        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
        std::string str(size_needed, '\0');
        WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), str.data(), size_needed, nullptr, nullptr);
        return str;
    }
}