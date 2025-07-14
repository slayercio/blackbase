#pragma once
#include <blackbase/cave/cave.hpp>

namespace blackbase
{
    BLACKBASE_API BLACKBASE_CONSTEXPR Cave::Cave()
        : m_Address(0), m_Flags(SectionFlags::None)
    {
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR Cave::Cave(std::uintptr_t address, blackbase::SectionFlags flags)
        : m_Address(address), m_Flags(flags)
    {
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR Cave::Cave(const Cave& other)
        : m_Address(other.m_Address), m_Flags(other.m_Flags)
    {
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR Cave::Cave(Cave&& other) noexcept
        : m_Address(other.m_Address), m_Flags(other.m_Flags)
    {
        other.m_Address = 0;
        other.m_Flags = SectionFlags::None;
    }

    BLACKBASE_API Cave& Cave::operator=(const Cave& other)
    {
        if (this != &other)
        {
            m_Address = other.m_Address;
            m_Flags = other.m_Flags;
        }
        return *this;
    }

    BLACKBASE_API Cave& Cave::operator=(Cave&& other) noexcept
    {
        if (this != &other)
        {
            m_Address = other.m_Address;
            m_Flags = other.m_Flags;
            other.m_Address = 0;
            other.m_Flags = SectionFlags::None;
        }
        return *this;
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR std::uintptr_t Cave::GetAddress() const
    {
        return m_Address;
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR blackbase::SectionFlags Cave::GetFlags() const
    {
        return m_Flags;
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR bool Cave::IsReadable() const
    {
        return static_cast<std::uint32_t>(m_Flags) & static_cast<uint32_t>(SectionFlags::Readable);
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR bool Cave::IsWritable() const
    {
        return static_cast<std::uint32_t>(m_Flags) & static_cast<uint32_t>(SectionFlags::Writable);
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR bool Cave::IsExecutable() const
    {
        return static_cast<std::uint32_t>(m_Flags) & static_cast<uint32_t>(SectionFlags::Executable);
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR void Cave::SetAddress(std::uintptr_t address)
    {
        m_Address = address;
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR void Cave::SetFlags(blackbase::SectionFlags flags)
    {
        m_Flags = flags;
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR blackbase::SectionFlags Cave::AddFlag(blackbase::SectionFlags flag)
    {
        m_Flags = static_cast<blackbase::SectionFlags>(static_cast<std::uint32_t>(m_Flags) | static_cast<std::uint32_t>(flag));
        return m_Flags;
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR blackbase::SectionFlags Cave::RemoveFlag(blackbase::SectionFlags flag)
    {
        m_Flags = static_cast<blackbase::SectionFlags>(static_cast<std::uint32_t>(m_Flags) & ~static_cast<std::uint32_t>(flag));
        return m_Flags;
    }
}