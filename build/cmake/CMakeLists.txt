cmake_minimum_required(VERSION 3.10)

include(GNUInstallDirs)

set(name ruis)

project(${name})

# ============================
# = find dependency packages =

# TODO: dependency list
find_package(Freetype CONFIG REQUIRED)
find_package(utki CONFIG REQUIRED)
find_package(papki CONFIG REQUIRED)
# find_package(ZLIB REQUIRED)
# find_package(r4 CONFIG REQUIRED)
# find_package(svgren CONFIG REQUIRED)
# find_package(tml CONFIG REQUIRED)
# find_package(rasterimage CONFIG REQUIRED)
# find_package(veg CONFIG REQUIRED)

# ============================

file(GLOB_RECURSE srcs "../../src/${name}/*.cpp")

add_library(
    ${name}
    STATIC
    ${srcs}
)

target_compile_features(${name} PUBLIC cxx_std_17)
set_target_properties(${name} PROPERTIES CXX_STANDARD_REQUIRED ON)
set_target_properties(${name} PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(
    ${name}
    INTERFACE
        $<BUILD_INTERFACE:>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(
    ${name}
    PUBLIC
    # TODO: dependency list
        Freetype::Freetype
        utki::utki
        papki::papki
        # r4::r4
        # svgren::svgren
        # tml::tml
        # rasterimage::rasterimage
        # veg::veg
)

# install library header files preserving directory hierarchy
install(
    DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}/../../src/${name}"
    DESTINATION
        "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING PATTERN
        "*.hpp"
)

install(
    TARGETS
        ${name}
    EXPORT # generate cmake configs
        ${name}-config
)

# install cmake configs
install(
    EXPORT
        ${name}-config
    FILE
        ${name}-config.cmake
    DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/${name}
    NAMESPACE
        ${name}::
)
