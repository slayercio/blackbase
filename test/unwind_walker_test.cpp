#include <blackbase/pattern/matcher.hpp>
#include <blackbase/unwind_walker.hpp>

int main()
{
    blackbase::Pattern p("4D 8B C8 C6 44 24");
    blackbase::Matcher m("ntdll.dll");

    auto ntdll = blackbase::Library::FindByName("ntdll.dll");
    if (!ntdll.has_value())
    {
        return 1;
    }

    blackbase::UnwindWalker w("ntdll.dll");
    auto exp = ntdll->GetExport("DbgPrintEx");
    if (!exp.has_value())
    {
        return 1;
    }
    
    auto match = m.FindFirst(p);
    if (!match.has_value())
    {
        return 1;
    }

    auto entry = w.FindFunctionEntry(match->GetMatchAddress());
    if (!entry.has_value())
    {
        return 1;
    }

    return entry->StartAddress == exp->GetAddress() ? 0 : 1;
}