#pragma once
#include <blackbase/pattern/matcher.hpp>
#include <blackbase/pattern/match.hpp>
#include <blackbase/library/library.hpp>
#include <blackbase/internal/log.hpp>
#include <blackbase/internal/defs/windows.hpp>
#include <blackbase/internal/assert.hpp>

namespace blackbase::pattern
{
    BLACKBASE_API BLACKBASE_CONSTEXPR Matcher::Matcher()
        : Matcher(0, 0)
    {
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR Matcher::Matcher(const std::uintptr_t moduleBase, const std::size_t moduleSize)
        : m_moduleBase(moduleBase), m_moduleSize(moduleSize)
    {
    }

    BLACKBASE_API Matcher::Matcher(const std::string_view& moduleName)
    {
        setModule(moduleName);
    }

    BLACKBASE_API void Matcher::setModule(const std::string_view& moduleName)
    {
        auto library = ::blackbase::library::Library::GetLibraryHandle(moduleName);
        
        BLACKBASE_ASSERT(library.has_value(), "Module '{}' not found", moduleName);

        m_moduleBase = library->GetHandle();
        m_moduleSize = library->GetSize();
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR void Matcher::setModule(const std::uintptr_t moduleBase, const std::size_t moduleSize)
    {
        m_moduleBase = moduleBase;
        m_moduleSize = moduleSize;
    }

    BLACKBASE_API std::vector<Match> Matcher::findAll(const Pattern& pattern) const
    {
        std::vector<Match> matches;

        size_t currentAddress = m_moduleBase;
        size_t endAddress = m_moduleBase + m_moduleSize;
        __win::MEMORY_BASIC_INFORMATION mbi;

        while (currentAddress + pattern.getBytes().size() < endAddress)
        {
            if (!VirtualQueryWrapper(reinterpret_cast<void*>(currentAddress), &mbi, sizeof(mbi)))
            {
                BLACKBASE_TRACE("VirtualQuery failed at address: 0x{:X}", currentAddress);
                currentAddress += 0x1000;
                continue;
            }

            size_t regionEnd = reinterpret_cast<size_t>(mbi.BaseAddress) + mbi.RegionSize;

            if (regionEnd > endAddress)
            {
                regionEnd = endAddress;
            }

            if (mbi.State == MEM_COMMIT && !(mbi.Protect & PAGE_NOACCESS) && !(mbi.Protect & PAGE_GUARD))
            {
                size_t bytesToCheck = regionEnd - currentAddress;

                const uint8_t* address = reinterpret_cast<uint8_t*>(currentAddress);

                for (size_t i = 0; i <= bytesToCheck - pattern.getBytes().size(); i++)
                {
                    bool match = true;

                    for (size_t j = 0; j < pattern.getBytes().size(); j++)
                    {
                        if (pattern.getMask()[j] && pattern.getBytes()[j] != address[i + j])
                        {
                            match = false;
                            break;
                        }
                    }

                    if (match)
                    {
                        matches.emplace_back(reinterpret_cast<std::uintptr_t>(address + i));
                    }
                }

                currentAddress = reinterpret_cast<size_t>(mbi.BaseAddress) + mbi.RegionSize;
                if (currentAddress <= reinterpret_cast<size_t>(mbi.BaseAddress))
                {
                    BLACKBASE_TRACE("Invalid memory region detected at address: 0x{:X}", currentAddress);
                    break; // Prevent infinite loop in case of invalid memory region
                }
            }
        }

        return matches;
    }

    BLACKBASE_API std::optional<Match> Matcher::findFirst(const Pattern& pattern) const
    {
        size_t currentAddress = m_moduleBase;
        size_t endAddress = m_moduleBase + m_moduleSize;
        __win::MEMORY_BASIC_INFORMATION mbi;

        while (currentAddress + pattern.getBytes().size() < endAddress)
        {
            if (!VirtualQueryWrapper(reinterpret_cast<void*>(currentAddress), &mbi, sizeof(mbi)))
            {
                BLACKBASE_TRACE("VirtualQuery failed at address: 0x{:X}", currentAddress);
                currentAddress += 0x1000;
                continue;
            }

            size_t regionEnd = reinterpret_cast<size_t>(mbi.BaseAddress) + mbi.RegionSize;

            if (regionEnd > endAddress)
            {
                regionEnd = endAddress;
            }

            if (mbi.State == MEM_COMMIT && !(mbi.Protect & PAGE_NOACCESS) && !(mbi.Protect & PAGE_GUARD))
            {
                size_t bytesToCheck = regionEnd - currentAddress;

                const uint8_t* address = reinterpret_cast<uint8_t*>(currentAddress);

                for (size_t i = 0; i <= bytesToCheck - pattern.getBytes().size(); i++)
                {
                    bool match = true;

                    for (size_t j = 0; j < pattern.getBytes().size(); j++)
                    {
                        if (pattern.getMask()[j] && pattern.getBytes()[j] != address[i + j])
                        {
                            match = false;
                            break;
                        }
                    }

                    if (match)
                    {
                        return Match(reinterpret_cast<std::uintptr_t>(address + i));
                    }
                }

                currentAddress = reinterpret_cast<size_t>(mbi.BaseAddress) + mbi.RegionSize;
                if (currentAddress <= reinterpret_cast<size_t>(mbi.BaseAddress))
                {
                    BLACKBASE_TRACE("Invalid memory region detected at address: 0x{:X}", currentAddress);
                    break; // Prevent infinite loop in case of invalid memory region
                }
            }
        }

        return std::nullopt;
    }

    BLACKBASE_API std::optional<Match> Matcher::verifyFirst(std::uintptr_t address, const Pattern& pattern) const
    {
        if (address < m_moduleBase || address >= m_moduleBase + m_moduleSize)
        {
            BLACKBASE_TRACE("Address 0x{:X} is out of module bounds", address);
            return std::nullopt;
        }

        auto matcher = Matcher(address, pattern.getBytes().size());
        auto match = matcher.findFirst(pattern);

        if (match.has_value() && match->getAddress() == address)
        {
            return match;
        }

        return std::nullopt;
    }

    BLACKBASE_API std::optional<Match> Matcher::verifyOrFind(std::uintptr_t address, const Pattern& pattern) const
    {
        auto match = verifyFirst(address, pattern);
        if (match.has_value())
        {
            return match;
        }

        return findFirst(pattern);
    }
}