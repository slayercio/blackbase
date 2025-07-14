#pragma once
#ifdef BLACKBASE_ENABLE_DISASSEMBLER
    #ifdef BLACKBASE_USE_ZYDIS
        #include <blackbase/detail/disassembler/dissassembler_zydis.hpp>
    #elif BLACKBASE_USE_UD2
        #include <blackbase/detail/disassembler/disassembler_ud2.hpp>
    #endif
#endif