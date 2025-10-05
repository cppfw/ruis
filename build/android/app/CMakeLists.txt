cmake_minimum_required(VERSION 3.4.1)

set(name ruisapp_application)
project(${name})

file(GLOB_RECURSE srcs
        "../../../tests/app/src/*.cpp"
        "../../../src/*.cpp"
        "../../../tests/harness/modules/ruisapp/src/*.cpp"
        "../../../tests/harness/modules/ruis-render-opengles/src/*.cpp"
    )

include (${ANDROID_GRADLE_NATIVE_BUNDLE_PLUGIN_MK})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -DDEBUG")

include_directories(
    ../../../src
    ../../../tests/harness/modules/ruisapp/src
    ../../../tests/harness/modules/ruis-render-opengles/src
)

add_library(
        # Specifies the name of the library.
        ${name} # this has to be 'ruisapp_application' so that RuisappActivity loads that named .so

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${srcs}
    )

include_directories(
        ../../src/
        ../../tests/harness/modules/ruisapp/src
        ../../tests/harness/modules/ruis-render-opengles/src
)

message(LINK = ${ANDROID_GRADLE_NATIVE_MODULES})

target_link_libraries(
        ${name}
        android log EGL z GLESv2 ${ANDROID_GRADLE_NATIVE_MODULES}
    )
