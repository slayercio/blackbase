#include <blackbase/blackbase.hpp>
#include <blackbase/library/library.hpp>
#include <iostream>
#include <print>

int main()
{
    auto library = blackbase::library::Library::GetLibraryHandle("kernel32.dll");

    if(library == std::nullopt)
    {
        std::println("Failed to get library handle.");
        return 1;
    }

    auto handle = library->GetHandle();
    auto name = library->GetName();

    std::println("Library Handle: {:#018x}", handle);
    std::println("Library Name: {}", name);

    if (library->IsValid())
    {
        std::println("Library is valid.");
    }
    else
    {
        std::println("Library is not valid.");
        return 1;
    }

    auto exportFunc = library->GetExport("Sleep");
    if (exportFunc == std::nullopt)
    {
        std::println("Export not found.");
        return 1;
    }
    else
    {
        std::println("Export found, name: {}, address: {:#018x}",
                     exportFunc->GetName(),
                     reinterpret_cast<std::uintptr_t>(exportFunc->GetAddress()));
    }

    std::println("Calling Sleep(1000)...");
    exportFunc->Call<void, unsigned long>(1000);

    auto libraryThatHasSleep = blackbase::library::Library::GetLibraryHandle(reinterpret_cast<std::uintptr_t>(exportFunc->GetAddress()));
    if (libraryThatHasSleep == std::nullopt)
    {
        std::println("Failed to get library handle from export address.");
        return 1;
    }

    std::println("Library from export address: {}", libraryThatHasSleep->GetName());

    return 0;
}