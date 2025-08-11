#pragma once
#include <blackbase/common.hpp>
#include <blackbase/string_convert.hpp>
#include <blackbase/library/export.hpp>

namespace blackbase
{
    class Library
    {
    private:
        std::uintptr_t m_BaseAddress;
        std::uintptr_t m_EndAddress;
        std::string    m_Name;

    public:
        static BLACKBASE_FORCEINLINE std::optional<Library> FindByName(const std::string_view& name) BLACKBASE_NOEXCEPT;
        static BLACKBASE_FORCEINLINE std::optional<Library> FindByAddress(std::uintptr_t address) BLACKBASE_NOEXCEPT;
        static BLACKBASE_FORCEINLINE std::vector<Library> FindAll() BLACKBASE_NOEXCEPT;

        static BLACKBASE_FORCEINLINE std::optional<Library> GetCurrent() BLACKBASE_NOEXCEPT;

    public:
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR Library() BLACKBASE_NOEXCEPT = default;
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR Library(std::uintptr_t baseAddress, std::uintptr_t endAddress, const std::string& name) BLACKBASE_NOEXCEPT;

    public:
        BLACKBASE_FORCEINLINE std::optional<Export> GetExport(const std::string_view& name) const BLACKBASE_NOEXCEPT;

    public:
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR std::uintptr_t GetBaseAddress() const BLACKBASE_NOEXCEPT;
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR std::uintptr_t GetEndAddress() const BLACKBASE_NOEXCEPT;
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR const std::string& GetName() const BLACKBASE_NOEXCEPT;
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR const std::size_t GetSize() const BLACKBASE_NOEXCEPT;
        BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR bool IsValid() const BLACKBASE_NOEXCEPT;
    };
}

#pragma region Implementation
namespace blackbase
{
    #include <Windows.h>
    #include <intrin.h>
    #include <blackbase/windows.hpp>

    namespace windows
    {
        inline PPEB GetPeb() BLACKBASE_NOEXCEPT
        {
            #if defined(_M_X64)
                PTEB teb = reinterpret_cast<PTEB>(__readgsqword(reinterpret_cast<DWORD_PTR>(&static_cast<NT_TIB*>(nullptr)->Self)));
            #elif defined(_M_IX86)
                PTEB teb = reinterpret_cast<PTEB>(__readfsdword(reinterpret_cast<DWORD_PTR>(&static_cast<NT_TIB*>(nullptr)->Self)));    
            #endif

            return teb->ProcessEnvironmentBlock;
        }

        inline void ForEachLibrary(const std::function<bool(PLDR_DATA_TABLE_ENTRY)>& callback) BLACKBASE_NOEXCEPT
        {
            auto peb = GetPeb();
            if (!peb || !peb->Ldr)
                return;

            auto ldr = peb->Ldr;
            if (!ldr)
                return;

            for (auto entry = ldr->InMemoryOrderModuleList.Flink; entry != &ldr->InMemoryOrderModuleList; entry = entry->Flink)
            {
                auto module = CONTAINING_RECORD(entry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

                if (callback(module))
                {
                    break;
                }
            }
        }
    }

    BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR Library::Library(std::uintptr_t baseAddress, std::uintptr_t endAddress, const std::string& name) BLACKBASE_NOEXCEPT
        : m_BaseAddress(baseAddress), m_EndAddress(endAddress), m_Name(name)
    {
    }

    BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR std::uintptr_t Library::GetBaseAddress() const BLACKBASE_NOEXCEPT
    {
        return m_BaseAddress;
    }

    BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR std::uintptr_t Library::GetEndAddress() const BLACKBASE_NOEXCEPT
    {
        return m_EndAddress;
    }

    BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR const std::string& Library::GetName() const BLACKBASE_NOEXCEPT
    {
        return m_Name;
    }

    BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR const std::size_t Library::GetSize() const BLACKBASE_NOEXCEPT
    {
        return m_EndAddress - m_BaseAddress;
    }

    BLACKBASE_FORCEINLINE BLACKBASE_CONSTEXPR bool Library::IsValid() const BLACKBASE_NOEXCEPT
    {
        return m_BaseAddress != 0 && m_EndAddress > m_BaseAddress;
    }

    BLACKBASE_FORCEINLINE std::optional<Library> Library::FindByName(const std::string_view& name) BLACKBASE_NOEXCEPT
    {
        std::optional<Library> library = std::nullopt;

        windows::ForEachLibrary([&](PLDR_DATA_TABLE_ENTRY entry) -> bool
        {
            auto myEntry = reinterpret_cast<blackbase::windows::PLDR_DATA_TABLE_ENTRY>(entry);
            auto wideName = ToWideString(name);

            if (_wcsicmp(myEntry->BaseDllName.Buffer, wideName.c_str()) == 0)
            {
                library = Library(
                    reinterpret_cast<std::uintptr_t>(myEntry->DllBase),
                    reinterpret_cast<std::uintptr_t>(myEntry->DllBase) + myEntry->SizeOfImage,
                    ToString(std::wstring_view(myEntry->BaseDllName.Buffer, myEntry->BaseDllName.Length / sizeof(wchar_t)))
                );

                return true; // Stop iteration
            }

            return false; // Continue iteration
        });

        return library;
    }

    BLACKBASE_FORCEINLINE std::optional<Library> Library::FindByAddress(std::uintptr_t address) BLACKBASE_NOEXCEPT
    {
        std::optional<Library> library = std::nullopt;

        windows::ForEachLibrary([&](PLDR_DATA_TABLE_ENTRY entry) -> bool
        {
            auto myEntry = reinterpret_cast<blackbase::windows::PLDR_DATA_TABLE_ENTRY>(entry);

            if (address >= reinterpret_cast<std::uintptr_t>(myEntry->DllBase) &&
                address < (reinterpret_cast<std::uintptr_t>(myEntry->DllBase) + myEntry->SizeOfImage))
            {
                library = Library(
                    reinterpret_cast<std::uintptr_t>(myEntry->DllBase),
                    reinterpret_cast<std::uintptr_t>(myEntry->DllBase) + myEntry->SizeOfImage,
                    ToString(std::wstring_view(myEntry->BaseDllName.Buffer, myEntry->BaseDllName.Length / sizeof(wchar_t)))
                );

                return true; // Stop iteration
            }

            return false; // Continue iteration
        });

        return library;
    }

    BLACKBASE_FORCEINLINE std::vector<Library> Library::FindAll() BLACKBASE_NOEXCEPT
    {
        std::vector<Library> libraries;

        windows::ForEachLibrary([&](PLDR_DATA_TABLE_ENTRY entry) -> bool
        {
            auto myEntry = reinterpret_cast<blackbase::windows::PLDR_DATA_TABLE_ENTRY>(entry);

            libraries.emplace_back(
                reinterpret_cast<std::uintptr_t>(myEntry->DllBase),
                reinterpret_cast<std::uintptr_t>(myEntry->DllBase) + myEntry->SizeOfImage,
                ToString(std::wstring_view(myEntry->BaseDllName.Buffer, myEntry->BaseDllName.Length / sizeof(wchar_t)))
            );

            return false; // Continue iteration
        });

        return libraries;
    }

    BLACKBASE_FORCEINLINE std::optional<Export> Library::GetExport(const std::string_view& name) const BLACKBASE_NOEXCEPT
    {
        if (!IsValid())
        {
            return std::nullopt;
        }

        auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(m_BaseAddress);
        if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        {
            return std::nullopt; // Not a valid DOS header
        }

        auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(m_BaseAddress + dosHeader->e_lfanew);
        if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
        {
            return std::nullopt; // Not a valid NT header
        }

        auto exportDirectory = &ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
        if (exportDirectory->VirtualAddress == 0 || exportDirectory->Size == 0)
        {
            return std::nullopt; // No export directory
        }

        auto exportTable = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(m_BaseAddress + exportDirectory->VirtualAddress);

        auto names = reinterpret_cast<PDWORD>(m_BaseAddress + exportTable->AddressOfNames);
        auto functions = reinterpret_cast<PDWORD>(m_BaseAddress + exportTable->AddressOfFunctions);
        auto ordinals = reinterpret_cast<PDWORD>(m_BaseAddress + exportTable->AddressOfNameOrdinals);

        for (size_t i = 0; i < exportTable->NumberOfNames; ++i)
        {
            const char* exportName = reinterpret_cast<const char*>(m_BaseAddress + names[i]);

            if (strcmpi(exportName, name.data()) == 0)
            {
                uint16_t ordinal = ordinals[i];
                uint32_t rva = functions[ordinal];

                std::uintptr_t address = m_BaseAddress + rva;
                return Export(exportName, address);
            }
        }

        return std::nullopt;
    }

    BLACKBASE_FORCEINLINE std::optional<Library> Library::GetCurrent() BLACKBASE_NOEXCEPT
    {
        std::optional<Library> currentLibrary = std::nullopt;

        // current module is the first module in the list
        windows::ForEachLibrary([&](PLDR_DATA_TABLE_ENTRY entry) -> bool
        {
            auto myEntry = reinterpret_cast<blackbase::windows::PLDR_DATA_TABLE_ENTRY>(entry);

            if (myEntry->BaseDllName.Length > 0 && myEntry->BaseDllName.Buffer)
            {
                currentLibrary = Library(
                    reinterpret_cast<std::uintptr_t>(myEntry->DllBase),
                    reinterpret_cast<std::uintptr_t>(myEntry->DllBase) + myEntry->SizeOfImage,
                    ToString(std::wstring_view(myEntry->BaseDllName.Buffer, myEntry->BaseDllName.Length / sizeof(wchar_t)))
                );

                return true; // Stop iteration
            }

            return false; // Continue iteration
        });

        return currentLibrary;
    }
}
#pragma endregion