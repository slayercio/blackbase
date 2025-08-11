#include <blackbase/library/library.hpp>
#include <iostream>
#include <print>

void TestExport(const std::string_view& libraryName, const std::string_view& function)
{
    try
    {
        auto library = blackbase::Library::FindByName(libraryName);
        if (!library)
        {
            std::cerr << xorstr_("Library not found: ") << libraryName << std::endl;
            return;
        }

        auto exportFunc = library->GetExport(function);
        if (!exportFunc)
        {
            std::cerr << xorstr_("Export not found: ") << function << xorstr_(" in ") << libraryName << std::endl;
            return;
        }

        std::cout << xorstr_("Export found: ") << exportFunc->GetName() << xorstr_(" at address: ") << std::hex << exportFunc->GetAddress() << std::dec << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

int main()
{
    TestExport(xorstr_("kernel32.dll"), xorstr_("GetModuleHandleA"));
    TestExport(xorstr_("user32.dll"), xorstr_("MessageBoxA"));
    TestExport(xorstr_("nonexistent.dll"), xorstr_("SomeFunction"));
    
    return 0;
}