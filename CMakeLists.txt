cmake_minimum_required(VERSION 4.0)
project(BlackBase VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_library(blackbase INTERFACE)
target_include_directories(blackbase INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

include(cmake/modes.cmake)
include(cmake/throws.cmake)

set(BLACKBASE_PCH_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/blackbase/common.hpp")

if (BLACKBASE_HEADER_ONLY)
    include(cmake/header_only.cmake)
else()
    include(cmake/compiled.cmake)
endif()

include(cmake/disassembler.cmake)
include(cmake/tests.cmake)
include(cmake/install.cmake)