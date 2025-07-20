#pragma once
#include <blackbase/pattern/match.hpp>
#include <blackbase/internal/defs/windows.hpp>

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

    BLACKBASE_API auto GetProtection(std::uintptr_t address) -> std::uint32_t
    {
        __win::MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQueryWrapper(reinterpret_cast<void*>(address), &mbi, sizeof(mbi)) == 0)
        {
            return 0; // Error retrieving memory information
        }

        return mbi.Protect;
    }

    BLACKBASE_API bool Match::IsExecutable(const Match& match)
    {
        auto protection = GetProtection(match.getAddress());

        return (protection & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) != 0;
    }

    BLACKBASE_API bool Match::IsExecutable(Match& match)
    {
        return IsExecutable(static_cast<const Match&>(match));
    }

    BLACKBASE_API bool Match::IsWritable(const Match& match)
    {
        auto protection = GetProtection(match.getAddress());

        return (protection & (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE)) != 0;
    }

    BLACKBASE_API bool Match::IsWritable(Match& match)
    {
        return IsWritable(static_cast<const Match&>(match));
    }

    BLACKBASE_API bool Match::IsReadable(const Match& match)
    {
        auto protection = GetProtection(match.getAddress());

        return (protection & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)) != 0;
    }

    BLACKBASE_API bool Match::IsReadable(Match& match)
    {
        return IsReadable(static_cast<const Match&>(match));
    }
}