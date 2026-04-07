#pragma once
#include <blackbase/library/library.hpp>

namespace blackbase
{
    class UnwindWalker
    {
    private:
        std::optional<Library> m_Library;

    public:
        UnwindWalker(const std::string& module_name);
        UnwindWalker(const Library& library);

    public:
        struct FunctionEntry
        {
            std::uint64_t StartAddress;
            std::uint64_t EndAddress;
        };

    public:
        std::optional<FunctionEntry> FindFunctionEntry(std::uint64_t address) const;
        std::vector<FunctionEntry> GetAllEntries() const;
    };
}

#pragma region Implementation
#ifdef _WIN32
namespace blackbase
{
    inline UnwindWalker::UnwindWalker(const std::string& module_name)
        : m_Library(Library::FindByName(module_name))
    {
    }

    inline UnwindWalker::UnwindWalker(const Library& library)
        : m_Library(library)
    {
    }

    inline std::optional<UnwindWalker::FunctionEntry> UnwindWalker::FindFunctionEntry(std::uint64_t address) const
    {
        if (!m_Library.has_value())
        {
            return std::nullopt;
        }

        PIMAGE_DOS_HEADER dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(m_Library->GetBaseAddress());
        if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
        {
            return std::nullopt;
        }

        PIMAGE_NT_HEADERS64 nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS64>(m_Library->GetBaseAddress() + dos_header->e_lfanew);
        if (nt_headers->Signature != IMAGE_NT_SIGNATURE)
        {
            return std::nullopt;
        }

        IMAGE_DATA_DIRECTORY unwind_directory = nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];
        if (unwind_directory.VirtualAddress == 0 || unwind_directory.Size == 0)
        {
            return std::nullopt;
        }

        auto base = m_Library->GetBaseAddress();
        auto end = m_Library->GetEndAddress();

        if (address < base || address >= end)
        {
            return std::nullopt;
        }

        auto pdata = reinterpret_cast<PIMAGE_RUNTIME_FUNCTION_ENTRY>(base + unwind_directory.VirtualAddress);
        auto count = unwind_directory.Size / sizeof(IMAGE_RUNTIME_FUNCTION_ENTRY);

        auto searched_rva = address - base;
        std::size_t left = 0;
        std::size_t right = count;

        while (left < right)
        {
            std::size_t mid = left + (right - left) / 2;
            auto entry = pdata[mid];

            if (searched_rva < entry.BeginAddress)
            {
                right = mid;
            }
            else if (searched_rva >= entry.EndAddress)
            {
                left = mid + 1;
            }
            else
            {
                return FunctionEntry
                { 
                    .StartAddress = base + entry.BeginAddress, 
                    .EndAddress = base + entry.EndAddress
                };
            }
        }

        return std::nullopt;
    }

    inline std::vector<UnwindWalker::FunctionEntry> UnwindWalker::GetAllEntries() const
    {
        if (!m_Library.has_value())
        {
            return {};
        }

        PIMAGE_DOS_HEADER dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(m_Library->GetBaseAddress());
        if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
        {
            return {};
        }

        PIMAGE_NT_HEADERS64 nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS64>(m_Library->GetBaseAddress() + dos_header->e_lfanew);
        if (nt_headers->Signature != IMAGE_NT_SIGNATURE)
        {
            return {};
        }

        IMAGE_DATA_DIRECTORY unwind_directory = nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];
        if (unwind_directory.VirtualAddress == 0 || unwind_directory.Size == 0)
        {
            return {};
        }

        auto base = m_Library->GetBaseAddress();
        auto pdata = reinterpret_cast<PIMAGE_RUNTIME_FUNCTION_ENTRY>(base + unwind_directory.VirtualAddress);
        auto count = unwind_directory.Size / sizeof(IMAGE_RUNTIME_FUNCTION_ENTRY);

        std::vector<FunctionEntry> entries;
        entries.reserve(count);

        for (std::size_t i = 0; i < count; ++i)
        {
            auto entry = pdata[i];
            entries.push_back(FunctionEntry
            {
                .StartAddress = base + entry.BeginAddress,
                .EndAddress = base + entry.EndAddress
            });
        }

        return entries;
    }
}
#endif
#pragma endregion