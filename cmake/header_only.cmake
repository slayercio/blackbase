target_compile_definitions(blackbase 
    INTERFACE BLACKBASE_HEADER_ONLY
    INTERFACE $<$<CONFIG:Debug>:DEBUG>
    INTERFACE $<$<CONFIG:Release>:NDEBUG>
)

if (BLACKBASE_ASSERTS_THROW)
    target_compile_definitions(blackbase
        INTERFACE BLACKBASE_ASSERTS_THROW
    )
endif()

if (BLACKBASE_ERRORS_THROW)
    target_compile_definitions(blackbase
        INTERFACE BLACKBASE_ERRORS_THROW
    )
endif()