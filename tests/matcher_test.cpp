#include <blackbase/blackbase.hpp>
#include <blackbase/pattern/pattern.hpp>
#include <blackbase/pattern/matcher.hpp>
#include <blackbase/library/library.hpp>
#include <print>

int main()
{
    auto library = blackbase::library::Library::GetLibraryHandle("kernel32.dll");
    if (!library)
    {
        std::println("Failed to get library handle.");
        return 1;   
    }

    auto exportedFunc = library->GetExport("Sleep");
    if (!exportedFunc)
    {
        std::println("Export not found.");
        return 1;
    }

    auto pattern = blackbase::pattern::Pattern("48 FF 25 C1 77 05 00");

    auto matcher = blackbase::pattern::Matcher(library->GetHandle(), library->GetSize());
    auto match = matcher.findFirst(pattern);

    if (!match)
    {
        std::println("Pattern not found.");
        return 1;
    }

    std::println("Pattern found at address: {:#018x}, valid = {}", match->getAddress(), match->getAddress() == reinterpret_cast<std::uintptr_t>(exportedFunc->GetAddress()));

    auto thunkPattern = blackbase::pattern::Pattern("48 FF 25 ? ? ? ?");
    auto thunkMatches = matcher.findAll(thunkPattern);

    std::println("Found {} thunk matches:", thunkMatches.size());
    for (const auto& thunkMatch : thunkMatches)
    {
        std::println("Thunk match at address: {:#018x}", thunkMatch.getAddress());
    }
}