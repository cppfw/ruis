# if the library is compiled by vcpkg during the port build (i.e. during the package installation),
# then we don't need to build test apps
if(IS_VCPKG_PORT_BUILD)
    return()
endif()

# no test apps for ios
if(IOS)
    return()
endif()

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
