#pragma once
#include <blackbase/common.hpp>
#include <blackbase/internal/constexpr.hpp>

namespace blackbase::pattern
{
    class Pattern
    {
    private:
        std::vector<std::uint8_t> m_Bytes;
        std::vector<bool>         m_Mask;

    public:
        BLACKBASE_API BLACKBASE_CONSTEXPR Pattern();

        /**
         * Constructs a pattern from a string representation.
         * @param pattern The byte pattern as a string (e.g., "A1 B2 C3").
         * @param mask The mask for the pattern (e.g., "x ? ?").
         */
        BLACKBASE_API BLACKBASE_CONSTEXPR Pattern(const std::string_view& pattern, const std::string_view& mask);

        /**
         * Constructs a pattern from a combination of bytes and mask.
         * @param combo The combination string (e.g., "A1 ? C3 ?").
         */
        BLACKBASE_API BLACKBASE_CONSTEXPR Pattern(const std::string_view& combo);

        BLACKBASE_API BLACKBASE_CONSTEXPR const std::vector<std::uint8_t>& getBytes() const noexcept;
        BLACKBASE_API BLACKBASE_CONSTEXPR const std::vector<bool>& getMask() const noexcept;

    private:
        BLACKBASE_API BLACKBASE_CONSTEXPR void parsePattern(const std::string_view& pattern, const std::string_view& mask);
        BLACKBASE_API BLACKBASE_CONSTEXPR void parseComboPattern(const std::string_view& pattern);
    };
}