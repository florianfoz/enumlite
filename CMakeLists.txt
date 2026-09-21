cmake_minimum_required(VERSION 3.20)

project(enumlite
    VERSION 1.0.0
    DESCRIPTION "Lightweight C++20 header-only enum and flags library"
    HOMEPAGE_URL "https://github.com/florianfoz/enumlite"
    LANGUAGES CXX
)

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------

option(ENUMLITE_BUILD_TESTS
    "Build enumlite tests"
    OFF
)

option(ENUMLITE_BUILD_EXAMPLES
    "Build enumlite examples"
    OFF
)

# ---------------------------------------------------------------------------
# Library
# ---------------------------------------------------------------------------

add_library(enumlite INTERFACE)

add_library(enumlite::enumlite ALIAS enumlite)

target_include_directories(enumlite
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(enumlite
    INTERFACE
        cxx_std_20
)

# ---------------------------------------------------------------------------
# Installation
# ---------------------------------------------------------------------------

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(
    TARGETS enumlite
    EXPORT enumliteTargets
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT enumliteTargets
    FILE enumliteTargets.cmake
    NAMESPACE enumlite::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/enumlite
)

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/enumliteConfig.cmake.in
    ${PROJECT_BINARY_DIR}/enumliteConfig.cmake
    INSTALL_DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/enumlite
)

write_basic_package_version_file(
    ${PROJECT_BINARY_DIR}/enumliteConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        ${PROJECT_BINARY_DIR}/enumliteConfig.cmake
        ${PROJECT_BINARY_DIR}/enumliteConfigVersion.cmake
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/enumlite
)

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------

if(ENUMLITE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ---------------------------------------------------------------------------
# Examples
# ---------------------------------------------------------------------------

if(ENUMLITE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()