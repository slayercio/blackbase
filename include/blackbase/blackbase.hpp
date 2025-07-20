#pragma once

#include <blackbase/cave/cave.hpp>
#include <blackbase/cave/cave_finder.hpp>
#include <blackbase/library/library.hpp>
#include <blackbase/library/export.hpp>
#include <blackbase/pattern/pattern.hpp>
#include <blackbase/pattern/match.hpp>
#include <blackbase/pattern/matcher.hpp>
#include <blackbase/util/string.hpp>
#include <blackbase/hooking/hook_manager.hpp>
#include <blackbase/hooking/hook.hpp>
#include <blackbase/hooking/vtable.hpp>

#if defined(BLACKBASE_HEADER_ONLY) || defined(BLACKBASE_BUILD)

    #include <blackbase/detail/pattern/pattern_impl.hpp>
    #include <blackbase/detail/pattern/match_impl.hpp>
    #include <blackbase/detail/pattern/matcher_impl.hpp>
    #include <blackbase/detail/util/string_impl.hpp>
    #include <blackbase/detail/library/library_impl.hpp>
    #include <blackbase/detail/library/export_impl.hpp>
    #include <blackbase/detail/cave/cave_impl.hpp>
    #include <blackbase/detail/cave/cave_finder_impl.hpp>
    #include <blackbase/detail/hooking/hook_manager_impl.hpp>
    #include <blackbase/detail/hooking/vtable_impl.hpp>

    #if defined(BLACKBASE_ENABLE_DISASSEMBLER)
        #include <blackbase/detail/disassembler/disassembler_impl.hpp>
    #endif

#endif