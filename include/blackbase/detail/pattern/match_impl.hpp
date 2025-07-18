#pragma once
#include <blackbase/pattern/match.hpp>

namespace blackbase::pattern
{
    BLACKBASE_API BLACKBASE_CONSTEXPR Match::Match()
        : m_Original(0), m_Address(0)
    {
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR Match::Match(std::uintptr_t address)
        : m_Original(address), m_Address(address)
    {
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR std::uintptr_t Match::getAddress() const
    {
        return m_Address;
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR std::uintptr_t Match::getOriginal() const
    {
        return m_Original;
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR bool Match::isValid() const
    {
        return m_Address != 0;
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR Match::operator bool() const
    {
        return isValid();
    }

    BLACKBASE_API Match& Match::resolveRelative(size_t opSize)
    {
        int32_t offset = *reinterpret_cast<int32_t*>(m_Address + opSize);
        m_Address += opSize + sizeof(offset); // Move past the instruction size and the offset
        m_Address += offset; // Apply the relative offset

        return *this;
    }

    BLACKBASE_API Match& Match::move(std::ptrdiff_t offset)
    {
        m_Address += offset;
        
        return *this;
    }
}