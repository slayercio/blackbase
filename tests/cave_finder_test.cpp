#include <blackbase/blackbase.hpp>
#include <blackbase/cave/cave_finder.hpp>
#include <print>
#include <cstdint>

extern "C" {
    #define MAX_PATH 260

    __declspec(dllimport) unsigned long __stdcall GetModuleFileNameA(
        void* hModule,
        char* lpFilename,
        unsigned long nSize
    );
}

void find_caves_in_module(blackbase::ICaveFinder* cave_finder, char* moduleName)
{
    std::print("Module Name: {}\n", moduleName);

    auto caves = cave_finder->findCaves(moduleName, 0x10);
    if (!caves.empty()) {
        std::print("Found {} caves:\n", caves.size());
        for (const auto& c : caves) {
            if (c.IsWritable())
            {
                std::print("Cave at address: 0x{:X}, writeable: {}, readable: {}, executable: {}\n", 
                    c.GetAddress(), c.IsWritable(), c.IsReadable(), c.IsExecutable());
            }
        }
    } else {
        std::print("No caves found.\n");
    }
}

void find_caves_in_self(blackbase::ICaveFinder* cave_finder)
{
    char moduleName[MAX_PATH];
    GetModuleFileNameA(NULL, moduleName, MAX_PATH);
    
    find_caves_in_module(cave_finder, moduleName);
}

int main()
{
    auto cave_finder = blackbase::CreateCaveFinder();

    if (!cave_finder) {
        std::print("Failed to create cave finder.\n");
        return 1;
    }

    std::string moduleName = "kernel32.dll";

    find_caves_in_self(cave_finder);
    find_caves_in_module(cave_finder, const_cast<char*>(moduleName.c_str()));
}