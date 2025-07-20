install(DIRECTORY include/ DESTINATION include)

install(TARGETS blackbase
    EXPORT BlackBaseTargets
)

if (TARGET blackbase_obj)
    install(TARGETS blackbase_obj EXPORT BlackBaseTargets)
endif()

if(TARGET blackbase_asm)
    install(TARGETS blackbase_asm EXPORT BlackBaseTargets)
endif()

install(EXPORT BlackBaseTargets
    FILE BlackBaseTargets.cmake
    NAMESPACE BlackBase::
    DESTINATION lib/cmake/BlackBase
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/BlackBaseConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/BlackBaseConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/BlackBaseConfig.cmake"
    INSTALL_DESTINATION lib/cmake/BlackBase
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/BlackBaseConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/BlackBaseConfigVersion.cmake"
    DESTINATION lib/cmake/BlackBase
)