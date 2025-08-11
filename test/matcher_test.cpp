#define NOMINMAX
#include <blackbase/pattern/matcher.hpp>
#include <iostream>

void TestMatcher(const std::string_view& module, const std::string_view& pattern) {
    try
    {
        blackbase::Pattern p(pattern);
        blackbase::Matcher m(module);

        auto matches = m.FindAll(p);

        for (const auto& match : matches) {
            std::cout << xorstr_("Match found at address: ") << std::hex << match.GetCurrentAddress() << std::dec << std::endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void ResolveThunks(const std::string_view& module)
{
    try
    {
        blackbase::Pattern p(xorstr_("48 FF 25 ? ? ? ?"));
        blackbase::Matcher m(module);

        auto matches = m.FindAll(p);

        for (size_t i = 0; i < std::min(matches.size(), 10ULL); ++i)
        {
            auto& match = matches[i];
            
            std::cout << xorstr_("Thunk found at address: ") << std::hex << match.GetMatchAddress() << " points to: " <<
                match.ResolveRelative().As<uintptr_t*>() << std::dec << std::endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

int main()
{
    TestMatcher(xorstr_("kernel32.dll"), xorstr_("48 FF 25 C1 77 05 00")); // sleep
    ResolveThunks(xorstr_("kernel32.dll")); // resolve thunks
}