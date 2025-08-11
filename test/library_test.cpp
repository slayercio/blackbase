#include <blackbase/library/library.hpp>
#include <iostream>

auto PrintLibraryInfo(const blackbase::Library& library)
{
    std::cout << xorstr_("Library Name: ") << library.GetName() << std::endl;
    std::cout << xorstr_("Base Address: ") << std::hex << library.GetBaseAddress() << std::dec << std::endl;
    std::cout << xorstr_("End Address: ") << std::hex << library.GetEndAddress() << std::dec << std::endl;
}

auto TestLibrary(const std::string_view& libraryName)
{
    try
    {
        auto library = blackbase::Library::FindByName(libraryName);
        if (library.has_value())
        {
            PrintLibraryInfo(*library);
        }
        else
        {
            std::cerr << xorstr_("Library not found: ") << libraryName << std::endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

int main()
{
    // Test with a known library name
    TestLibrary(xorstr_("kernel32.dll"));

    // Test with an invalid library name
    TestLibrary(xorstr_("nonexistent.dll"));

    auto allLibraries = blackbase::Library::FindAll();
    std::cout << xorstr_("Total Libraries Found: ") << allLibraries.size() << std::endl;
    for (const auto& lib : allLibraries)
    {
        PrintLibraryInfo(lib);
    }

    int x = 10;

    auto libraryByAddress = blackbase::Library::FindByAddress(reinterpret_cast<std::uintptr_t>(&TestLibrary));
    if (libraryByAddress.has_value())
    {
        PrintLibraryInfo(*libraryByAddress);
    }
    else
    {
        std::cerr << xorstr_("Library not found by address.") << std::endl;
    }

    return 0;
}