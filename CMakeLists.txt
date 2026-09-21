cmake_minimum_required(VERSION 3.20)

project(enumlite
    VERSION 1.0.0
    DESCRIPTION "Lightweight C++20 header-only enum and flags library"
    HOMEPAGE_URL "https://github.com/florianfoz/enumlite"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# ------------------------------------------------------------------------------
# Options
# ------------------------------------------------------------------------------

option(ENUMLITE_BUILD_QT
    "Build/install the Qt backend"
    OFF
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------------------------
# Core library
# ------------------------------------------------------------------------------

add_library(enumlite INTERFACE)

add_library(enumlite::enumlite ALIAS enumlite)

target_compile_features(enumlite
    INTERFACE
        cxx_std_20
)

target_include_directories(enumlite
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# ------------------------------------------------------------------------------
# Qt backend
# ------------------------------------------------------------------------------

if(ENUMLITE_BUILD_QT)

    find_package(Qt6 REQUIRED COMPONENTS Core)

    add_library(enumlite_qt INTERFACE)

    add_library(enumlite::enumlite_qt ALIAS enumlite_qt)

    target_link_libraries(enumlite_qt
        INTERFACE
            enumlite::enumlite
            Qt6::Core
    )

    target_include_directories(enumlite_qt
        INTERFACE
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

endif()

# ------------------------------------------------------------------------------
# Installation
# ------------------------------------------------------------------------------

install(
    TARGETS
        enumlite
    EXPORT
        enumliteTargets
)

if(ENUMLITE_BUILD_QT)
    install(
        TARGETS
            enumlite_qt
        EXPORT
            enumliteTargets
    )
endif()

install(
    DIRECTORY
        include/
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT
        enumliteTargets
    FILE
        enumliteTargets.cmake
    NAMESPACE
        enumlite::
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/enumlite
)

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/enumliteConfig.cmake.in
    ${PROJECT_BINARY_DIR}/enumliteConfig.cmake
    INSTALL_DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/enumlite
)

write_basic_package_version_file(
    ${PROJECT_BINARY_DIR}/enumliteConfigVersion.cmake
    VERSION
        ${PROJECT_VERSION}
    COMPATIBILITY
        SameMajorVersion
)

install(
    FILES
        ${PROJECT_BINARY_DIR}/enumliteConfig.cmake
        ${PROJECT_BINARY_DIR}/enumliteConfigVersion.cmake
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/enumlite
)


option(ENUMLITE_BUILD_TESTS
    "Build enumlite tests"
    OFF
)

if(ENUMLITE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()