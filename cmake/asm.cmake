enable_language(ASM_MASM)

if (BLACKBASE_HEADER_ONLY)
    add_library(blackbase_asm STATIC src/indirect_call.asm)
else()
    target_sources(blackbase_obj
        PRIVATE
            src/indirect_call.asm 
    )
endif()