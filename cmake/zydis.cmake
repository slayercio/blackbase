add_subdirectory(vendor/zydis)

target_link_libraries(blackbase INTERFACE 
    Zydis
)

if (NOT BLACKBASE_HEADER_ONLY)
    target_include_directories(blackbase_obj PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vendor/zydis/include>
    )

    target_link_libraries(blackbase_obj PRIVATE 
        Zydis
    )
endif()