# if the library is compiled by vcpkg during the port build (i.e. during the package installation),
# then we don't need to build test apps
if(IS_VCPKG_PORT_BUILD)
    return()
endif()

# no test apps for ios
if(IOS)
    return()
endif()

if(NOT CPPFW_MONOREPO)
    return()
endif()

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

function(ruis_declare_app name)
    set(options HAS_RESOURCES)
    set(single)
    set(multiple)
    cmake_parse_arguments(arg "${options}" "${single}" "${multiple}" ${ARGN})

    set(srcs)
    myci_add_source_files(srcs
        DIRECTORY
            ${CMAKE_CURRENT_LIST_DIR}/../../tests/${name}/src
        RECURSIVE
    )

    if(arg_HAS_RESOURCES)
        set(resources_dir ../../tests/${name}/res)
    endif()

    myci_declare_application(ruis-${name}-app-opengl
        GUI
        SOURCES
            ${srcs}
        RESOURCE_DIRECTORY
            ${resources_dir}
        DEPENDENCIES
            ruisapp::ruisapp-opengl
    )

    # not all platforms have OpenGL ES, so check if opengles variant of ruisapp is defined
    if(TARGET ruisapp-opengles)
        myci_declare_application(ruis-${name}-app-opengles
            GUI
            SOURCES
                ${srcs}
            RESOURCE_DIRECTORY
                ${resources_dir}
            DEPENDENCIES
                ruisapp::ruisapp-opengles
        )
    endif()
endfunction()

ruis_declare_app(align)
ruis_declare_app(app HAS_RESOURCES)
ruis_declare_app(app2 HAS_RESOURCES)
ruis_declare_app(aspect_ratio_proxy)
ruis_declare_app(book HAS_RESOURCES)
ruis_declare_app(easing)
ruis_declare_app(paint)
ruis_declare_app(tabbed_book)
ruis_declare_app(wire_area)
