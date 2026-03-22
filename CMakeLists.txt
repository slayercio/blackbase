cmake_minimum_required(VERSION 4.0)
project(BlackBase VERSION 2.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(blackbase INTERFACE)
target_include_directories(blackbase INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set(BLACKBASE_PCH_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/blackbase/common.hpp")

target_compile_definitions(blackbase 
    INTERFACE $<$<CONFIG:Debug>:DEBUG>
    INTERFACE $<$<CONFIG:Release>:NDEBUG>
)

option(BLACKBASE_BUILD_TESTS "Build tests for BlackBase" ON)
option(BLACKBASE_USE_STD_STACKTRACE "Use std::stacktrace for debugging" ON)

option(BLACKBASE_IMPL_AVX2 "Enable AVX2 optimizations in pattern matcher (if supported at runtime)" ON)
option(BLACKBASE_IMPL_SSE2 "Enable SSE2 optimizations in pattern matcher (if supported at runtime)" ON)

if (BLACKBASE_IMPL_AVX2)
    target_compile_definitions(blackbase 
        INTERFACE BLACKBASE_IMPL_AVX2
    )
endif()

if (BLACKBASE_IMPL_SSE2)
    target_compile_definitions(blackbase 
        INTERFACE BLACKBASE_IMPL_SSE2
    )
endif()

if (BLACKBASE_USE_STD_STACKTRACE)
    target_compile_definitions(blackbase 
        INTERFACE USE_STD_STACKTRACE
    )

    target_link_libraries(blackbase
        INTERFACE
            Dbghelp.lib
    )
endif()

if (BLACKBASE_BUILD_TESTS)
    file(GLOB_RECURSE TEST_SOURCES "test/*.cpp")
    foreach(TEST_SOURCE ${TEST_SOURCES})
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
        add_executable(${TEST_NAME} ${TEST_SOURCE})
        target_include_directories(${TEST_NAME} 
            PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )
        target_link_libraries(${TEST_NAME} blackbase)
        target_compile_definitions(${TEST_NAME}
            PUBLIC $<$<CONFIG:Debug>:DEBUG>
            PUBLIC $<$<CONFIG:Release>:NDEBUG>
        )
    endforeach()
endif()