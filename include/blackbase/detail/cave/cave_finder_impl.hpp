#pragma once
#include <blackbase/cave/cave_finder.hpp>
#include <blackbase/library/library.hpp>
#include <blackbase/internal/defs/windows.hpp>

namespace blackbase
{
    BLACKBASE_API std::vector<std::uintptr_t> FindCaves(std::uintptr_t start, std::uintptr_t end, size_t requiredSize, bool breakOnFirst = true)
    {
        std::vector<std::uintptr_t> caves;

        for (std::uintptr_t address = start; address < end; address++)
        {
            if (*reinterpret_cast<std::uint8_t*>(address) == 0x00)
            {
                std::size_t size = 0;
                while (address + size < end && *reinterpret_cast<std::uint8_t*>(address + size) == 0x00)
                {
                    size++;
                    if (size >= requiredSize)
                    {
                        caves.push_back(address);
                        if (breakOnFirst)
                        {
                            return caves; // Return the first found cave
                        }
                        
                        break; // Found a cave, break to continue searching for more
                    }
                }

                address += size; // Skip to the end of the current cave
            }
        }

        return caves; // Return all found caves
    }

    BLACKBASE_API blackbase::SectionFlags GetSectionFlags(std::uintptr_t address)
    {
        __win::MEMORY_BASIC_INFORMATION mbi{};

        if (VirtualQueryWrapper(reinterpret_cast<__win::PVOID>(address), &mbi, sizeof(mbi)) == 0)
        {
            return blackbase::SectionFlags::None; // Error retrieving memory info
        }

        uint32_t flags = static_cast<uint32_t>(blackbase::SectionFlags::None);
        if (mbi.Protect & PAGE_EXECUTE || mbi.Protect & PAGE_EXECUTE_READ ||
            mbi.Protect & PAGE_EXECUTE_READWRITE || mbi.Protect & PAGE_EXECUTE_WRITECOPY)
        {
            flags |= static_cast<uint32_t>(blackbase::SectionFlags::Executable);
        }

        if (mbi.Protect & PAGE_READONLY || mbi.Protect & PAGE_READWRITE ||
            mbi.Protect & PAGE_WRITECOPY || mbi.Protect & PAGE_EXECUTE_READ || mbi.Protect & PAGE_EXECUTE_READWRITE)
        {
            flags |= static_cast<uint32_t>(blackbase::SectionFlags::Readable);
        }

        if (mbi.Protect & PAGE_READWRITE || mbi.Protect & PAGE_WRITECOPY ||
            mbi.Protect & PAGE_EXECUTE_READWRITE || mbi.Protect & PAGE_EXECUTE_WRITECOPY)
        {
            flags |= static_cast<uint32_t>(blackbase::SectionFlags::Writable);
        }

        return static_cast<blackbase::SectionFlags>(flags);
    }

    class CaveFinderWin : public ICaveFinder
    {
    public:
        CaveFinderWin() = default;
        ~CaveFinderWin() override = default;

        BLACKBASE_API std::optional<Cave> findCave(const std::string& moduleName, size_t requiredSize) const override
        {
            auto library = blackbase::library::Library::GetLibraryHandle(moduleName);
            if (!library)
            {
                return std::nullopt; // Library not found
            }

            auto handle = library->GetHandle();
            auto size = library->GetSize();
            auto caves = FindCaves(handle, handle + size, requiredSize, true);

            if (caves.empty())
            {
                return std::nullopt; // No cave found
            }

            auto flags = GetSectionFlags(caves.front());
            return Cave(caves.front(), flags);
        }

        BLACKBASE_API std::vector<Cave> findCaves(const std::string& moduleName, size_t requiredSize) const override
        {
            auto library = blackbase::library::Library::GetLibraryHandle(moduleName);
            if (!library)
            {
                return {}; // Library not found
            }

            auto handle = library->GetHandle();
            auto size = library->GetSize();
            auto caves = FindCaves(handle, handle + size, requiredSize, false);

            std::vector<Cave> foundCaves;
            for (const auto& caveAddress : caves)
            {
                auto flags = GetSectionFlags(caveAddress);
                foundCaves.emplace_back(caveAddress, flags);
            }

            return foundCaves; // Return all found caves
        }
    };

    BLACKBASE_API ICaveFinder* CreateCaveFinder()
    {
        return new CaveFinderWin();
    }
}