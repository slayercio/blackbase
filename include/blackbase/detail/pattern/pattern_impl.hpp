#pragma once
#include <blackbase/pattern/pattern.hpp>
#include <blackbase/internal/assert.hpp>
#include <blackbase/internal/log.hpp>

namespace blackbase::pattern
{
    static BLACKBASE_CONSTEXPR bool is_space_char(char c) noexcept
    {
        return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
    }

    static BLACKBASE_CONSTEXPR bool is_hex_char(char c) noexcept
    {
        return (c >= '0' && c <= '9') ||
               (c >= 'A' && c <= 'F') ||
               (c >= 'a' && c <= 'f');
    }

    static BLACKBASE_CONSTEXPR std::uint8_t parse_hex_char(char c)
    {
        if (c >= '0' && c <= '9')
        {
            return static_cast<std::uint8_t>(c - '0');
        }
        else if (c >= 'a' && c <= 'f')
        {
            return static_cast<std::uint8_t>(c - 'a' + 10);
        }
        else if (c >= 'A' && c <= 'F')
        {
            return static_cast<std::uint8_t>(c - 'A' + 10);
        }

        BLACKBASE_ASSERT(false, "Invalid hex character");
        return 0; // This should never be reached due to the assert
    }

    static BLACKBASE_CONSTEXPR std::uint8_t parse_hex_byte(char h, char l)
    {
        BLACKBASE_ASSERT(is_hex_char(l), "Invalid hex character");
        BLACKBASE_ASSERT(is_hex_char(h), "Invalid hex character");

        return static_cast<std::uint8_t>((parse_hex_char(h) << 4) | parse_hex_char(l));
    }

    /**
     * @brief Parses the byte pattern and mask from the provided strings.
     * Supported pattern formats:
     * - "A1 B2 C3"
     * - "A1B2C3"
     * Supported mask formats:
     * - "x ? ?"
     * - "xx?"
     */
    BLACKBASE_API BLACKBASE_CONSTEXPR void Pattern::parsePattern(const std::string_view& pattern, const std::string_view& mask)
    {
        m_Bytes.clear();
        m_Mask.clear();

        size_t patternIndex = 0;
        auto skip_pattern_spaces = [&]()
        {
            while (patternIndex < pattern.size() && is_space_char(pattern[patternIndex]))
            {
                ++patternIndex;
            }
        };

        while (patternIndex < pattern.size())
        {
            skip_pattern_spaces();

            if (is_hex_char(pattern[patternIndex]))
            {
                skip_pattern_spaces();

                BLACKBASE_ASSERT(patternIndex + 1 < pattern.size() && is_hex_char(pattern[patternIndex + 1]),
                                 "Invalid hex byte in pattern");

                char high = pattern[patternIndex++];
                char low =  pattern[patternIndex++];

                m_Bytes.push_back(parse_hex_byte(high, low));
            }
            else
            {
                BLACKBASE_TRACE("Invalid character '{}' in pattern", pattern[patternIndex]);
                break;
            }
        }

        size_t maskIndex = 0;
        auto skip_mask_spaces = [&]()
        {
            while (maskIndex < mask.size() && is_space_char(mask[maskIndex]))
            {
                ++maskIndex;
            }
        };

        while (maskIndex < mask.size())
        {
            skip_mask_spaces();

            if (mask[maskIndex] == 'x' || mask[maskIndex] == 'X')
            {
                m_Mask.push_back(true);
                ++maskIndex;
            }
            else if (mask[maskIndex] == '?')
            {
                m_Mask.push_back(false);
                ++maskIndex;
            }
            else
            {
                BLACKBASE_TRACE("Invalid character '{}' in mask", mask[maskIndex]);
                break;
            }
        }

        BLACKBASE_ASSERT(m_Bytes.size() == m_Mask.size(),
                        "Pattern and mask sizes do not match: {} bytes, {} mask entries",
                        m_Bytes.size(), m_Mask.size());
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR void Pattern::parseComboPattern(const std::string_view& pattern)
    {
        size_t index = 0;
        auto skip_spaces = [&]()
        {
            while (index < pattern.length() && is_space_char(pattern[index]))
            {
                index++;
            }
        };

        m_Bytes.clear();
        m_Mask.clear();

        while (index < pattern.length())
        {
            skip_spaces();

            if (is_hex_char(pattern[index]))
            {
                BLACKBASE_ASSERT(index + 1 < pattern.length() && is_hex_char(pattern[index + 1]),
                                 "Invalid hex byte in combo pattern");

                char high = pattern[index++];
                char low =  pattern[index++];

                m_Bytes.push_back(parse_hex_byte(high, low));
                m_Mask.push_back(true); // Default mask for bytes
            }
            else if (pattern[index] == '?')
            {
                m_Bytes.push_back(0); // Placeholder for unknown byte
                m_Mask.push_back(false); // Mask indicates this byte is ignored
                index++;
            }
            else
            {
                BLACKBASE_TRACE("Invalid character '{}' in combo pattern", pattern[index]);
                break;
            }
        }
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR Pattern::Pattern() 
    {
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR Pattern::Pattern(const std::string_view& pattern, const std::string_view& mask)
    {
        parsePattern(pattern, mask);
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR Pattern::Pattern(const std::string_view& combo)
    {
        parseComboPattern(combo);
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR const std::vector<std::uint8_t>& Pattern::getBytes() const noexcept
    {
        return m_Bytes;
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR const std::vector<bool>& Pattern::getMask() const noexcept
    {
        return m_Mask;
    }
}