option(BLACKBASE_BUILD_TESTS "Build BlackBase tests" OFF)

if (BLACKBASE_BUILD_TESTS)
    file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")
    foreach(TEST_SOURCE ${TEST_SOURCES})
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
        add_executable(${TEST_NAME} ${TEST_SOURCE})
        target_include_directories(${TEST_NAME} 
            PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )
        target_link_libraries(${TEST_NAME} blackbase blackbase_asm)
        target_compile_definitions(${TEST_NAME}
            PUBLIC $<$<CONFIG:Debug>:DEBUG>
            PUBLIC $<$<CONFIG:Release>:NDEBUG>
        )
    endforeach()
endif()