#pragma once
#include <blackbase/common.hpp>

namespace blackbase
{
    class Match
    {
    private:
        std::uintptr_t m_CurrentAddress = 0;
        std::uintptr_t m_MatchAddress   = 0;

    public:
        constexpr static std::ptrdiff_t LoadEffectiveAddressOffset = 0x3;

    public:
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR Match() BLACKBASE_NOEXCEPT = default;
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR Match(std::uintptr_t address) BLACKBASE_NOEXCEPT;

    public:
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR std::uintptr_t GetCurrentAddress() const BLACKBASE_NOEXCEPT;
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR std::uintptr_t GetMatchAddress() const BLACKBASE_NOEXCEPT;

        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR bool IsValid() const BLACKBASE_NOEXCEPT;
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR operator bool() const BLACKBASE_NOEXCEPT;

    public:
        BLACKBASE_FORCEINLINE Match& ResolveRelative(std::ptrdiff_t offset = LoadEffectiveAddressOffset);

    public:
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR Match& Move(std::ptrdiff_t offset) BLACKBASE_NOEXCEPT;

    public:
        template <typename T, typename std::enable_if_t<std::is_pointer_v<T>, int> = 0>
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR T As() const BLACKBASE_NOEXCEPT
        {
            return reinterpret_cast<T>(m_CurrentAddress);
        }

        template <typename T, typename std::enable_if_t<!std::is_pointer_v<T>, int> = 0>
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR T As() const BLACKBASE_NOEXCEPT
        {
            return reinterpret_cast<T*>(m_CurrentAddress);
        }
    };
}

#pragma region Implementation
namespace blackbase
{
    BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR Match::Match(std::uintptr_t address) BLACKBASE_NOEXCEPT
        : m_CurrentAddress(address), m_MatchAddress(address)
    {
    }

    BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR std::uintptr_t Match::GetCurrentAddress() const BLACKBASE_NOEXCEPT
    {
        return m_CurrentAddress;
    }

    BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR std::uintptr_t Match::GetMatchAddress() const BLACKBASE_NOEXCEPT
    {
        return m_MatchAddress;
    }

    BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR bool Match::IsValid() const BLACKBASE_NOEXCEPT
    {
        return m_MatchAddress != 0;
    }

    BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR Match::operator bool() const BLACKBASE_NOEXCEPT
    {
        return IsValid();
    }

    BLACKBASE_FORCEINLINE Match& Match::ResolveRelative(std::ptrdiff_t offset)
    {
        std::int32_t relativeOffset = *reinterpret_cast<std::int32_t*>(m_CurrentAddress + offset);
        Move(offset);
        Move(sizeof(relativeOffset));
        Move(relativeOffset);

        return *this;
    }

    BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR Match& Match::Move(std::ptrdiff_t offset) BLACKBASE_NOEXCEPT
    {
        m_CurrentAddress += offset;
        
        return *this;
    }
}
#pragma endregion