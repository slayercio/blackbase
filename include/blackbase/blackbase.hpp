#pragma once

#if defined(BLACKBASE_HEADER_ONLY) || defined(BLACKBASE_BUILD)

    #include <blackbase/detail/pattern/pattern_impl.hpp>
    #include <blackbase/detail/pattern/match_impl.hpp>
    #include <blackbase/detail/pattern/matcher_impl.hpp>
    #include <blackbase/detail/util/string_impl.hpp>
    #include <blackbase/detail/library/library_impl.hpp>
    #include <blackbase/detail/library/export_impl.hpp>
    #include <blackbase/detail/cave/cave_impl.hpp>
    #include <blackbase/detail/cave/cave_finder_impl.hpp>

    #if defined(BLACKBASE_ENABLE_DISASSEMBLER)
        #include <blackbase/detail/disassembler/disassembler_impl.hpp>
    #endif

#endif