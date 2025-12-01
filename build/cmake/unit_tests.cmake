# if the library is compiled by vcpkg during the port build (i.e. during the package installation),
# then we don't need to build unit tests
if(IS_VCPKG_PORT_BUILD)
    return()
endif()

# no unit tests for ios
if(IOS)
    return()
endif()

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

    set(ruis_test_harness_dir ${CMAKE_CURRENT_LIST_DIR}/../../tests/harness)

    myci_add_source_files(ruis_render_null_srcs
        DIRECTORY
            ${ruis_test_harness_dir}/modules/ruis-render-null/src
        RECURSIVE
    )

    myci_declare_library(ruis-render-null
        SOURCES
            ${ruis_render_null_srcs}
        PUBLIC_INCLUDE_DIRECTORIES
            ${ruis_test_harness_dir}/modules/ruis-render-null/src
        DEPENDENCIES
            ruis
    )

    set(ruis_render_null_namespace ${PROJECT_NAME})
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
