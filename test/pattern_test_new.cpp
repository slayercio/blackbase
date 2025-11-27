#include <string_view>
#include <cstdint>
#include <array>
#include <cctype>
#include <cstddef>

#include <blackbase/xorstr.hpp>

constexpr bool IsHexChar(char c)
{
    return (c >= '0' && c <= '9') ||
           (c >= 'A' && c <= 'F') ||
           (c >= 'a' && c <= 'f');
}

constexpr int HexValue(char c)
{
    return (c >= '0' && c <= '9') ? (c - '0') :
        (c >= 'A' && c <= 'F') ? (c - 'A' + 10) :
        (c >= 'a' && c <= 'f') ? (c - 'a' + 10) :
        throw "Invalid hex character";
}

consteval std::size_t CountPatternBytes(std::string_view pattern)
{
    std::size_t count = 0;
    std::size_t i = 0;

    while (i < pattern.size())
    {
        while (i < pattern.size() && (pattern[i] == ' ' || pattern[i] == '\t'))
        {
            ++i;
        }

        if (i >= pattern.size())
        {
            break;
        }

        if (pattern[i] == '?')
        {
            ++count;
            if (i + 1 < pattern.size() && pattern[i + 1] == '?')
            {
                i += 2;
            }
            else
            {
                ++i;
            }
        }
        else if (IsHexChar(static_cast<unsigned char>(pattern[i])) && i + 1 < pattern.size() && IsHexChar(static_cast<unsigned char>(pattern[i + 1])))
        {
            ++count;
            i += 2;
        }
        else
        {
            throw "Invalid pattern format";
        }
    }

    return count;
}

template<std::size_t N>
struct Pattern
{
    std::array<std::uint8_t, N> m_Bytes{};
    std::array<bool, N> m_Mask{};
    std::size_t m_Size{};

    consteval Pattern(const char(&pattern)[N])
    {
        m_Size = 0;
        std::size_t i = 0;

        while (i < N - 1)
        {
            while (i < N - 1 && (pattern[i] == ' ' || pattern[i] == '\t'))
            {
                ++i;
            }

            if (i >= N - 1)
            {
                break;
            }

            if (pattern[i] == '?' && (pattern[i + 1] == '?'))
            {
                m_Bytes[m_Size] = 0x00;
                m_Mask[m_Size] = false;
                ++m_Size;
                i += 2;
            }
            else if (pattern[i] == '?')
            {
                m_Bytes[m_Size] = 0x00;
                m_Mask[m_Size] = false;
                ++m_Size;
                ++i;
            }
            else if (IsHexChar(static_cast<unsigned char>(pattern[i])) && IsHexChar(static_cast<unsigned char>(pattern[i + 1])))
            {
                uint8_t byte = (HexValue(pattern[i]) << 4) | HexValue(pattern[i + 1]);
                m_Bytes[m_Size] = byte;
                m_Mask[m_Size] = true;
                ++m_Size;
                i += 2;
            }
            else
            {
                throw "Invalid pattern format";
            }
        }
    }
};

template<std::size_t N>
void PrintPattern(const Pattern<N>& pattern)
{
    for (std::size_t i = 0; i < pattern.m_Size; ++i)
    {
        if (pattern.m_Mask[i])
        {
            printf("%02X ", pattern.m_Bytes[i]);
        }
        else
        {
            printf("?? ");
        }
    }
    printf("\n");
}

int main()
{
    constexpr auto pattern = Pattern("48 8B ?? ?? ?? 48 85 C0 AB CD E0 ? 90");
    constexpr auto patternSize = CountPatternBytes("48 8B ?? ?? ?? 48 85 C0 AB CD E0 ? 90 ?");


    PrintPattern(pattern);
    return 0;
}
