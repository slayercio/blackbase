#pragma once
#include <blackbase/library/library.hpp>
#include <blackbase/internal/defs/windows.hpp>
#include <blackbase/util/string.hpp>
#include <blackbase/internal/log.hpp>
#include <functional>


namespace blackbase::library
{
    BLACKBASE_API BLACKBASE_CONSTEXPR Library::Library()
        : m_Handle(0), m_Size(0), m_Name("") {}
    
    BLACKBASE_API BLACKBASE_CONSTEXPR Library::Library(std::uintptr_t handle, std::size_t size, const std::string_view& name)
        : m_Handle(handle), m_Size(size), m_Name(name) {}

    BLACKBASE_API BLACKBASE_CONSTEXPR Library::Library(const Library& other)
        : m_Handle(other.m_Handle), m_Size(other.m_Size), m_Name(other.m_Name) {}

    BLACKBASE_API BLACKBASE_CONSTEXPR Library& Library::operator=(const Library& other)
    {
        if (this != &other)
        {
            m_Handle = other.m_Handle;
            m_Size = other.m_Size;
            m_Name = other.m_Name;
        }
        return *this;
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR Library::Library(Library&& other) noexcept
        : m_Handle(other.m_Handle), m_Size(other.m_Size), m_Name(std::move(other.m_Name))
    {
        other.m_Handle = 0;
        other.m_Size = 0;
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR Library& Library::operator=(Library&& other) noexcept
    {
        if (this != &other)
        {
            m_Handle = other.m_Handle;
            m_Size = other.m_Size;
            m_Name = std::move(other.m_Name);

            other.m_Handle = 0;
            other.m_Size = 0;
        }

        return *this;
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR std::uintptr_t Library::GetHandle() const
    {
        return m_Handle;
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR const std::string& Library::GetName() const
    {
        return m_Name;
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR std::size_t Library::GetSize() const
    {
        return m_Size;
    }

    BLACKBASE_API BLACKBASE_CONSTEXPR bool Library::IsValid() const
    {
        return m_Handle != 0;
    }

    namespace __windows
    {
        PPEB GetPeb()
        {
            return reinterpret_cast<PPEB>(__readgsqword(0x60));
        }
    }

    BLACKBASE_API void ForeachLibrary(std::function<bool(PLDR_DATA_TABLE_ENTRY)> callback)
    {
        auto peb = __windows::GetPeb();
        if (!peb)
        {
            return;
        }

        auto ldr = peb->Ldr;
        if (!ldr)
        {
            return;
        }

        for (auto entry = ldr->InMemoryOrderModuleList.Flink; entry != &ldr->InMemoryOrderModuleList; entry = entry->Flink)
        {
            auto mod = reinterpret_cast<PLDR_DATA_TABLE_ENTRY>(
                ((LDR_DATA_TABLE_ENTRY*)((char*)(entry) - (__int64)(&((LDR_DATA_TABLE_ENTRY*)0)->InMemoryOrderLinks)))
            );

            if (callback(mod))
            {
                return;
            }
        }
    }

    BLACKBASE_API std::optional<Library> Library::GetLibraryHandle(const std::string_view& name)
    {
        BLACKBASE_DEBUG("Searching for library: {}", name);

        std::optional<Library> result;

        ForeachLibrary([&result, &name](PLDR_DATA_TABLE_ENTRY mod) mutable {
            auto wideName = util::ToWide(name);
            BLACKBASE_DEBUG("Checking module: {}, result = {}", util::ToNarrow(mod->BaseDllName.Buffer, mod->BaseDllName.Length / sizeof(wchar_t)), _wcsicmp(mod->BaseDllName.Buffer, wideName.c_str()));

            if (_wcsicmp(mod->BaseDllName.Buffer, wideName.c_str()) == 0)
            {
                result = Library(
                    reinterpret_cast<std::uintptr_t>(mod->DllBase),
                    mod->SizeOfImage,
                    name
                );

                return true;
            }

            return false;
        });

        return result;
    }

    BLACKBASE_API std::optional<Library> Library::GetLibraryHandle(std::uintptr_t address)
    {
        std::optional<Library> result;

        ForeachLibrary([&](PLDR_DATA_TABLE_ENTRY mod) {
            auto startAddress = reinterpret_cast<std::uintptr_t>(mod->DllBase);
            auto endAddress = startAddress + mod->SizeOfImage;
            bool isValid = address >= startAddress && address < endAddress;

            BLACKBASE_DEBUG("Checking module: {}, start: {:#018x}, end: {:#018x}, address: {:#018x}, result = {}", 
                util::ToNarrow(mod->BaseDllName.Buffer, mod->BaseDllName.Length / sizeof(wchar_t)), 
                startAddress, endAddress, address, isValid);

            if (isValid)
            {
                result = Library(
                    reinterpret_cast<std::uintptr_t>(mod->DllBase),
                    mod->SizeOfImage,
                    util::ToNarrow(mod->BaseDllName.Buffer, mod->BaseDllName.Length / sizeof(wchar_t))
                );

                return true;
            }

            return false;
        });

        return result;
    }

    BLACKBASE_API std::optional<blackbase::library::Export> Library::GetExport(const std::string_view& name) const
    {
        if (!IsValid())
        {
            return std::nullopt;
        }

        std::optional<blackbase::library::Export> result;
        auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(m_Handle);
        if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        {
            return std::nullopt;
        }

        auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS64>(reinterpret_cast<std::uintptr_t>(dosHeader) + dosHeader->e_lfanew);
        if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
        {
            return std::nullopt;
        }

        auto exportDir = &ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
        if (exportDir->VirtualAddress == 0 || exportDir->Size == 0)
        {
            return std::nullopt;
        }

        auto exportTable = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(
            reinterpret_cast<std::uintptr_t>(dosHeader) + exportDir->VirtualAddress
        );

        auto names = reinterpret_cast<PDWORD>(reinterpret_cast<std::uintptr_t>(dosHeader) + exportTable->AddressOfNames);
        auto ordinals = reinterpret_cast<PWORD>(reinterpret_cast<std::uintptr_t>(dosHeader) + exportTable->AddressOfNameOrdinals);
        auto functions = reinterpret_cast<PDWORD>(reinterpret_cast<std::uintptr_t>(dosHeader) + exportTable->AddressOfFunctions);

        for (uint32_t i = 0; i < exportTable->NumberOfNames; ++i)
        {
            const char* exportName = reinterpret_cast<const char*>(reinterpret_cast<std::uintptr_t>(dosHeader) + names[i]);
            BLACKBASE_DEBUG("Checking export: {} in {}", exportName, m_Name);
            
            if (strcmp(exportName, name.data()) == 0)
            {
                uint16_t ordinal = ordinals[i];
                uint32_t functionRVA = functions[ordinal];
                auto address = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(dosHeader) + functionRVA);
                
                return blackbase::library::Export(name, address);
            }
        }
       

        return std::nullopt;
    }
}