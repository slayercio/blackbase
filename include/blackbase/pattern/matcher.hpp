#pragma once
#include <blackbase/common.hpp>
#include <blackbase/pattern/pattern.hpp>
#include <blackbase/pattern/match.hpp>
#include <blackbase/library/library.hpp>

namespace blackbase
{
    class Matcher
    {
    private:
        std::uintptr_t m_ModuleBase;
        std::uintptr_t m_ModuleEnd;

    public:
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR Matcher() BLACKBASE_NOEXCEPT = default;
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR Matcher(std::uintptr_t moduleBase, std::uintptr_t moduleEnd) BLACKBASE_NOEXCEPT;

        BLACKBASE_FORCEINLINE Matcher(const std::string_view& moduleName) BLACKBASE_NOEXCEPT;

    public:
        BLACKBASE_FORCEINLINE std::vector<Match>   FindAll(const Pattern& pattern) const BLACKBASE_NOEXCEPT;
        BLACKBASE_FORCEINLINE std::optional<Match> FindFirst(const Pattern& pattern) const BLACKBASE_NOEXCEPT;
    };
}

#pragma region Implementation
namespace blackbase
{
    BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR Matcher::Matcher(std::uintptr_t moduleBase, std::uintptr_t moduleEnd) BLACKBASE_NOEXCEPT
        : m_ModuleBase(moduleBase), m_ModuleEnd(moduleEnd)
    {
    }

    BLACKBASE_FORCEINLINE Matcher::Matcher(const std::string_view& moduleName) BLACKBASE_NOEXCEPT
        : m_ModuleBase(0), m_ModuleEnd(0)
    {
        const auto library = Library::FindByName(moduleName);
        if (library.has_value())
        {
            m_ModuleBase = library->GetBaseAddress();
            m_ModuleEnd  = library->GetEndAddress();
        }
        else
        {
            runtime_throw<std::runtime_error>(xorstr_("Module not found: {}"), moduleName);
        }
    }

    BLACKBASE_FORCEINLINE std::vector<Match> Matcher::FindAll(const Pattern& pattern) const BLACKBASE_NOEXCEPT
    {
        if (m_ModuleBase == 0 || m_ModuleEnd == 0)
        {
            return {};
        }

        std::vector<Match> matches;
        size_t currentAddress = m_ModuleBase;
        size_t endAddress = m_ModuleEnd;

        MEMORY_BASIC_INFORMATION mbi;

        size_t patternSize = pattern.GetBytes().size();
        size_t pageSize = blackbase::windows::GetPageSize();

        auto patternBytes = pattern.GetBytes();
        auto patternMask = pattern.GetMask();

        while (currentAddress + patternSize < endAddress)
        {
            if (!VirtualQuery(reinterpret_cast<LPCVOID>(currentAddress), &mbi, sizeof(mbi)))
            {
                currentAddress += pageSize;
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

                const uint8_t* address = reinterpret_cast<const uint8_t*>(currentAddress);

                for (size_t i = 0; i <= bytesToCheck - patternSize; i++)
                {
                    bool match = true;

                    for(size_t j = 0; j < patternSize; j++)
                    {
                        if (patternMask[j] > 0 && patternBytes[j] != address[i + j])
                        {
                            match = false;
                            break;
                        }
                    }

                    if (match)
                    {
                        matches.emplace_back(currentAddress + i);
                    }
                }
            }

            currentAddress = reinterpret_cast<size_t>(mbi.BaseAddress) + mbi.RegionSize;
            if (currentAddress <= reinterpret_cast<size_t>(mbi.BaseAddress))
            {
                break; // Prevent infinite loop if the address does not progress
            }
        }

        return matches;
    }

    BLACKBASE_FORCEINLINE std::optional<Match> Matcher::FindFirst(const Pattern& pattern) const BLACKBASE_NOEXCEPT
    {
        if (m_ModuleBase == 0 || m_ModuleEnd == 0)
        {
            return std::nullopt;
        }

        size_t currentAddress = m_ModuleBase;
        size_t endAddress = m_ModuleEnd;

        MEMORY_BASIC_INFORMATION mbi;

        size_t patternSize = pattern.GetBytes().size();
        size_t pageSize = blackbase::windows::GetPageSize();

        auto patternBytes = pattern.GetBytes();
        auto patternMask = pattern.GetMask();

        while (currentAddress + patternSize < endAddress)
        {
            if (!VirtualQuery(reinterpret_cast<LPCVOID>(currentAddress), &mbi, sizeof(mbi)))
            {
                currentAddress += pageSize;
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

                const uint8_t* address = reinterpret_cast<const uint8_t*>(currentAddress);

                for (size_t i = 0; i <= bytesToCheck - patternSize; i++)
                {
                    bool match = true;

                    for(size_t j = 0; j < patternSize; j++)
                    {
                        if (patternMask[j] > 0 && patternBytes[j] != address[i + j])
                        {
                            match = false;
                            break;
                        }
                    }

                    if (match)
                    {
                        return Match{ currentAddress + i };
                    }
                }
            }

            currentAddress = reinterpret_cast<size_t>(mbi.BaseAddress) + mbi.RegionSize;
            if (currentAddress <= reinterpret_cast<size_t>(mbi.BaseAddress))
            {
                break; // Prevent infinite loop if the address does not progress
            }
        }

        return std::nullopt;
    }


}
#pragma endregion