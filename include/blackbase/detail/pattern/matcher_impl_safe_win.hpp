#pragma once
#include <blackbase/pattern/matcher.hpp>
#include <blackbase/pattern/match.hpp>
#include <blackbase/library/library.hpp>
#include <blackbase/internal/log.hpp>
#include <blackbase/internal/defs/windows.hpp>

namespace blackbase::pattern
{
    BLACKBASE_API std::vector<Match> Matcher::findAll(const Pattern& pattern) const
    {
        std::vector<Match> matches;

        size_t currentAddress = m_moduleBase;
        size_t endAddress = m_moduleBase + m_moduleSize;
        ::MEMORY_BASIC_INFORMATION mbi;

        while (currentAddress + pattern.getBytes().size() < endAddress)
        {
            if (!VirtualQuery(reinterpret_cast<void*>(currentAddress), &mbi, sizeof(mbi)))
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
        MEMORY_BASIC_INFORMATION mbi;

        while (currentAddress + pattern.getBytes().size() < endAddress)
        {
            if (!VirtualQuery(reinterpret_cast<void*>(currentAddress), &mbi, sizeof(mbi)))
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
}