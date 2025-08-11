#pragma once
#include <blackbase/common.hpp>
#include <blackbase/internal.hpp>

namespace blackbase
{
    class Pattern
    {
    private:
        /**
         * @brief Holds the known bytes of the pattern.
         */
        std::vector<std::uint8_t> m_Bytes;

        /**
         * @brief Holds the mask for the pattern.
         * The mask is used to ignore certain bytes in the pattern.
         */
        std::vector<std::uint8_t> m_Mask;

    public:
        /**
         * @brief Constructs a Pattern object with default values.
         * This constructor initializes the pattern with empty byte and mask vectors.
         */
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR Pattern() BLACKBASE_NOEXCEPT = default;

        /**
         * @brief Constructs a Pattern object with the specified pattern and mask.
         * @param pattern The byte pattern to be used (eg. "A1 B2 C3 00").
         * @param mask The mask corresponding to the pattern (eg. "x x ? ?").
         */
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR Pattern(const std::string_view& pattern, const std::string_view& mask);
    
        /**
         * @brief Constructs a Pattern object from a combo pattern string.
         * The combo pattern is a string that combines the pattern and mask.
         * @param comboPattern The combined pattern and mask string (eg. "A1 B2 ? C3 ?").
         */
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR Pattern(const std::string_view& comboPattern);

        /**
         * @brief Gets the byte pattern of the Pattern object.
         * @return A constant reference to the vector of bytes representing the pattern.
         */
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR const std::vector<std::uint8_t>& GetBytes() const BLACKBASE_NOEXCEPT;

        /**
         * @brief Gets the mask of the Pattern object.
         * The mask is used to indicate which bytes in the pattern are significant.
         * @return A constant reference to the vector of bytes representing the mask.
         */
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR const std::vector<std::uint8_t>& GetMask() const BLACKBASE_NOEXCEPT;

    private:
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR void ParsePattern(const std::string_view& pattern, const std::string_view& mask);
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR void ParseComboPattern(const std::string_view& comboPattern);
    };
}

#pragma region Pattern Implementation
namespace blackbase
{
    BLACKBASE_CONSTEXPR Pattern::Pattern(const std::string_view& pattern, const std::string_view& mask)
    {
        ParsePattern(pattern, mask);
    }

    BLACKBASE_CONSTEXPR Pattern::Pattern(const std::string_view& comboPattern)
    {
        ParseComboPattern(comboPattern);
    }

    BLACKBASE_CONSTEXPR const std::vector<std::uint8_t>& Pattern::GetBytes() const BLACKBASE_NOEXCEPT
    {
        return m_Bytes;
    }

    BLACKBASE_CONSTEXPR const std::vector<std::uint8_t>& Pattern::GetMask() const BLACKBASE_NOEXCEPT
    {
        return m_Mask;
    }

    BLACKBASE_CONSTEXPR void Pattern::ParsePattern(const std::string_view& pattern, const std::string_view& mask)
    {
        m_Bytes.clear();
        m_Mask.clear();

        std::string_view patternView = pattern;
        std::string_view maskView = mask;

        auto skipSpaces = [](std::string_view& sv, std::size_t& index)
        {
            while (!sv.empty() && std::isspace(static_cast<unsigned char>(sv.front())))
            {
                sv.remove_prefix(1);
                ++index;
            }
        };

        auto hexToByte = [](char hi, char lo) -> std::uint8_t
        {
            auto hexVal = [](char c) -> int
            {
                if (c >= '0' && c <= '9') return c - '0';
                if (c >= 'A' && c <= 'F') return c - 'A' + 10;
                if (c >= 'a' && c <= 'f') return c - 'a' + 10;
                runtime_throw<std::invalid_argument>(xorstr_("Invalid hex character '{}'"), c);
            };

            return static_cast<std::uint8_t>((hexVal(hi) << 4) | hexVal(lo));
        };

        size_t patternIndex = 0;
        size_t maskIndex = 0;

        while (true)
        {
            skipSpaces(patternView, patternIndex);
            skipSpaces(maskView, maskIndex);

            if (patternView.empty() || maskView.empty())
                break;

            char maskChar = maskView.front();
            maskView.remove_prefix(1);

            if (maskChar == 'x' || maskChar == 'X')
            {
                if (patternView.size() < 2)
                    runtime_throw<std::invalid_argument>(xorstr_("Invalid pattern format at offset {} (expected two hex digits)"), patternIndex);

                char h1 = patternView.front(); patternView.remove_prefix(1);
                char h2 = patternView.front(); patternView.remove_prefix(1);

                m_Bytes.push_back(hexToByte(h1, h2));
                m_Mask.push_back(1); // push a truthy value for set byte
            }
            else if (maskChar == '?')
            {
                if (patternView.size() < 2)
                    runtime_throw<std::invalid_argument>(xorstr_("Invalid pattern format at offset {} (expected two hex digits)"), patternIndex);

                patternView.remove_prefix(2); // skip the two hex digits
                m_Mask.push_back(0); // push a falsy value for ignored byte
                m_Bytes.push_back(0); // push a placeholder byte
            }
            else
            {
                runtime_throw<std::invalid_argument>(xorstr_("Invalid mask character '{}' at offset {}"), maskChar, maskIndex);
            }
        }

        if (!patternView.empty() || !maskView.empty())
            runtime_throw<std::invalid_argument>(xorstr_("Pattern and mask lengths do not match"));
    }

    BLACKBASE_CONSTEXPR void Pattern::ParseComboPattern(const std::string_view& comboPattern)
    {
        auto skipSpaces = [](std::string_view& sv, std::size_t& index)
        {
            while (!sv.empty() && std::isspace(static_cast<unsigned char>(sv.front())))
            {
                sv.remove_prefix(1);
                ++index;
            }
        };

        auto hexToByte = [](char hi, char lo) -> std::uint8_t
        {
            auto hexVal = [](char c) -> int
            {
                if (c >= '0' && c <= '9') return c - '0';
                if (c >= 'A' && c <= 'F') return c - 'A' + 10;
                if (c >= 'a' && c <= 'f') return c - 'a' + 10;
                runtime_throw<std::invalid_argument>(xorstr_("Invalid hex character '{}'"), c);
            };

            return static_cast<std::uint8_t>((hexVal(hi) << 4) | hexVal(lo));
        };

        std::string_view patternView = comboPattern;
        size_t index = 0;

        while (true)
        {
            skipSpaces(patternView, index);

            if (patternView.size() >= 2)
            {
                char h1 = patternView[0];
                char h2 = patternView[1];

                // wildcard ??
                if (h1 == '?' && h2 == '?')
                {
                    m_Bytes.push_back(0);
                    m_Mask.push_back(0); // push a falsy value for ignored byte
                    patternView.remove_prefix(2);
                }
                // wildcard ?
                else if (h1 == '?')
                {
                    m_Bytes.push_back(0);
                    m_Mask.push_back(0); // push a falsy value for ignored byte
                    patternView.remove_prefix(1);
                }
                else if (std::isxdigit(static_cast<unsigned char>(h1)) &&
                    (std::isxdigit(static_cast<unsigned char>(h2)) || h2 == '?'))
                {
                    if (!std::isxdigit(static_cast<unsigned char>(h2))) // invalid partial like F?
                    {
                        runtime_throw<std::invalid_argument>(xorstr_("Invalid pattern format at offset {} (expected two hex digits or '?')"), index);
                    }

                    m_Bytes.push_back(hexToByte(h1, h2));
                    m_Mask.push_back(1); // push a truthy value for set byte
                    patternView.remove_prefix(2);
                }
                else
                {
                    runtime_throw<std::invalid_argument>(xorstr_("Invalid pattern format at offset {} (expected two hex digits or '?')"), index);
                }
            }
            else if (patternView.size() == 1)
            {
                if (patternView[0] == '?')
                {
                    m_Bytes.push_back(0);
                    m_Mask.push_back(0); // push a falsy value for ignored byte
                    patternView.remove_prefix(1);
                }
                else
                {
                    runtime_throw<std::invalid_argument>(xorstr_("Invalid pattern format at offset {} (expected two hex digits or '?')"), index);
                }
            }
            else
            {
                break; // end of pattern
            }
        }
    }
}
#pragma endregion