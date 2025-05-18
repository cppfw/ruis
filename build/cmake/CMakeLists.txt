cmake_minimum_required(VERSION 3.20)

set(name ruis)
project(${name})

# !!! find_package must go after project() declaration !!!
# Otherwise VCPKG does not set the CMAKE_PREFIX_PATH to find packages.
find_package(myci CONFIG REQUIRED)

set(srcs)
myci_add_source_files(srcs
    DIRECTORY
        ../../src/${name}
    RECURSIVE
)

set(rsrs)
myci_add_resource_files(rsrs
    DIRECTORY
        ../../res/ruis_res
)

myci_declare_library(${name}
    SOURCES
        ${srcs}
    RESOURCES
        ${rsrs}
    PUBLIC_INCLUDE_DIRECTORIES
        ../../src
    INSTALL_INCLUDE_DIRECTORIES
        ../../src/${name}
    DEPENDENCIES
        rasterimage
        r4
        papki
        tml
        utki
        agg
        veg
        svgren
        Freetype
    EXTERNAL_DEPENDENCIES
        JPEG
)
