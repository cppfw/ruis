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

myci_declare_library(${name}
    SOURCES
        ${srcs}
    RESOURCE_DIRECTORY
        ../../res/ruis_res
    PUBLIC_INCLUDE_DIRECTORIES
        ../../src
    INSTALL_INCLUDE_DIRECTORIES
        ../../src/${name}
    DEPENDENCIES
        rasterimage
        r4
        fsif
        tml
        utki
        veg
        svgren
        Freetype
)

# =====================
# = test-applications =

if(CPPFW_MONOREPO)
    # In case of monorepo build we can add necessary dependencies right here.

    myci_add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../../ruis-render-opengl/build/cmake
        BINARY_DIR
            ruis-render-opengl
    )
    myci_add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../../ruis-render-opengles/build/cmake
        BINARY_DIR
            ruis-render-opengles
    )
    myci_add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../../ruisapp/build/cmake
        BINARY_DIR
            ruisapp
    )

    set(ruisapp_namespace ruisapp)
else()
    # In case of non-monorepo build, we need to build the dependencies
    # from sources (from git submodules) together with unit tests.

    # TODO:

    set(ruisapp_namespace ruis)
endif()

function(ruis_declare_app name)
    set(srcs)
    myci_add_source_files(srcs
        DIRECTORY
            ${CMAKE_CURRENT_LIST_DIR}/../../tests/${name}/src
        RECURSIVE
    )

    myci_declare_application(ruis-${name}-app-opengl
        GUI
        SOURCES
            ${srcs}
        RESOURCE_DIRECTORY
            ../../tests/${name}/res
        DEPENDENCIES
            ${ruisapp_namespace}::ruisapp-opengl
    )

    myci_declare_application(ruis-${name}-app-opengles
        GUI
        SOURCES
            ${srcs}
        RESOURCE_DIRECTORY
            ../../tests/${name}/res
        DEPENDENCIES
            ${ruisapp_namespace}::ruisapp-opengles
    )
endfunction()

ruis_declare_app(align)
ruis_declare_app(app)
ruis_declare_app(app2)
ruis_declare_app(aspect_ratio_proxy)
ruis_declare_app(book)
ruis_declare_app(easing)
ruis_declare_app(paint)
ruis_declare_app(tabbed_book)
ruis_declare_app(wire_area)

# ==============
# = unit tests =

if(CPPFW_MONOREPO)
    # In case of monorepo build we can add necessary dependencies right here.

    myci_add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../../ruis-render-null/build/cmake
        BINARY_DIR
            ruis-render-null
    )

    set(ruis_render_null_namespace ruis-render-null)
else()
    # In case of non-monorepo build, we need to build the dependencies
    # from sources (from git submodules) together with unit tests.

    # TODO:

    set(ruis_render_null_namespace ruis)
endif()

set(tests_srcs)
myci_add_source_files(tests_srcs
    DIRECTORY
        ${CMAKE_CURRENT_LIST_DIR}/../../tests/unit/src
    RECURSIVE
)
myci_add_source_files(tests_srcs
    DIRECTORY
        ${CMAKE_CURRENT_LIST_DIR}/../../tests/harness/util
    RECURSIVE
)

myci_declare_application(${PROJECT_NAME}-tests
    SOURCES
        ${tests_srcs}
    DEPENDENCIES
        tst
        ruis
        ${ruis_render_null_namespace}::ruis-render-null
)

myci_declare_test(${PROJECT_NAME}-tests)
