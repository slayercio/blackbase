file(GLOB_RECURSE BLACKBASE_SOURCES "src/**.cpp")

if(BLACKBASE_BUILD_SHARED)
    add_library(blackbase_obj SHARED ${BLACKBASE_SOURCES})
elseif(BLACKBASE_BUILD_STATIC)
    add_library(blackbase_obj STATIC ${BLACKBASE_SOURCES})
else()
    message(FATAL_ERROR "No build type specified. Set BLACKBASE_BUILD_SHARED or BLACKBASE_BUILD_STATIC.")
endif()

target_compile_definitions(blackbase_obj
    PUBLIC BLACKBASE_BUILD
    PUBLIC $<$<CONFIG:Debug>:DEBUG>
    PUBLIC $<$<CONFIG:Release>:NDEBUG>
)

if (BLACKBASE_ASSERTS_THROW)
    target_compile_definitions(blackbase_obj
        PUBLIC BLACKBASE_ASSERTS_THROW
    )
endif()

if (BLACKBASE_ERRORS_THROW)
    target_compile_definitions(blackbase_obj
        PUBLIC BLACKBASE_ERRORS_THROW
    )
endif()

target_include_directories(blackbase_obj
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_precompile_headers(blackbase_obj
    PRIVATE ${BLACKBASE_PCH_HEADER}
)

target_link_libraries(blackbase INTERFACE blackbase_obj)

set_target_properties(blackbase_obj PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    OUTPUT_NAME blackbase
)