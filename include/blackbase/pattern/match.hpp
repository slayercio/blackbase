#pragma once
#include <blackbase/common.hpp>

namespace blackbase::pattern
{
    class Match
    {
    private:
        std::uintptr_t m_Original;
        std::uintptr_t m_Address;

    public:
        BLACKBASE_API BLACKBASE_CONSTEXPR Match();
        BLACKBASE_API BLACKBASE_CONSTEXPR Match(std::uintptr_t address);

        /**
         * Gets the address of the match.
         * @return The address of the match.
         */
        BLACKBASE_API BLACKBASE_CONSTEXPR std::uintptr_t getAddress() const;

        /**
         * Gets the original address of the match.
         * @return The original address of the match.
         */
        BLACKBASE_API BLACKBASE_CONSTEXPR std::uintptr_t getOriginal() const;

        /**
         * Checks if the match is valid.
         * @return True if the match is valid, false otherwise.
         */
        BLACKBASE_API BLACKBASE_CONSTEXPR bool isValid() const;

        /**
         * Converts the match to a boolean value.
         * @return True if the match is valid, false otherwise.
         */
        BLACKBASE_API BLACKBASE_CONSTEXPR operator bool() const;

    public:
        static constexpr std::size_t LEA_SIZE = 3; // Size of the LEA instruction in bytes

        /**
         * Resolves the match to a relative address. (eg. op reg, [rip + offset])
         * @return The resolved relative address.
         */
        BLACKBASE_API Match& resolveRelative(std::size_t opSize = LEA_SIZE);

        BLACKBASE_API Match& move(std::ptrdiff_t offset);

    public:
        template <typename T, typename std::enable_if_t<!std::is_pointer_v<T>, int> = 0>
        BLACKBASE_CONSTEXPR T* as() const
        {
            return reinterpret_cast<T*>(m_Address);
        }

        template <typename T, typename std::enable_if_t<std::is_pointer_v<T>, int> = 0>
        BLACKBASE_CONSTEXPR T as() const
        {
            return reinterpret_cast<T>(m_Address);
        }
    };
}

#ifdef BLACKBASE_HEADER_ONLY
#include <blackbase/detail/pattern/match_impl.hpp>
#endif