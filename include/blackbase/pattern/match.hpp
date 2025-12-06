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
        inline Match() = default;
        inline Match(std::uintptr_t address) 
        {
            m_CurrentAddress = address;
            m_MatchAddress   = address;
        }

    public:
        inline std::uintptr_t GetCurrentAddress() const noexcept
        {
            return m_CurrentAddress;
        }
        
        inline std::uintptr_t GetMatchAddress() const noexcept 
        {
            return m_MatchAddress;
        }

        inline bool IsValid() const noexcept
        {
            return m_MatchAddress != 0;
        }

        inline operator bool() const noexcept
        {
            return IsValid();
        }

    public:
        inline Match& ResolveRelative(std::ptrdiff_t offset = LoadEffectiveAddressOffset)
        {
            std::int32_t relativeOffset = *reinterpret_cast<std::int32_t*>(m_CurrentAddress + offset);

            this->Move(offset);
            this->Move(sizeof(relativeOffset));
            this->Move(relativeOffset);

            return *this;
        }

    public:
        inline Match& Move(std::ptrdiff_t offset) noexcept
        {
            m_CurrentAddress += offset;
            return *this;
        }

    public:
        template <typename T>
        inline T As() const noexcept
        {
            // If T is not a pointer type, dereference the address to get the value.
            if constexpr (!std::is_pointer_v<T>)
            {
                return *reinterpret_cast<T*>(m_CurrentAddress);
            }
            else
            {
                return reinterpret_cast<T>(m_CurrentAddress);
            }
        }
    };
}