#pragma once
#include <blackbase/library/library.hpp>
#include <blackbase/internal/defs/windows.hpp>
#include <blackbase/util/string.hpp>
#include <blackbase/internal/log.hpp>
#include <functional>


namespace blackbase::library
{
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