cmake_minimum_required(VERSION 3.4.1)

set(name ruis)
project(${name})

file(GLOB_RECURSE srcs "../../../src/*.cpp")

include (${ANDROID_GRADLE_NATIVE_BUNDLE_PLUGIN_MK})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

add_library(
        # Specifies the name of the library.
        ${name}

        # Sets the library as a shared library.
        STATIC

        # Provides a relative path to your source file(s).
        ${srcs}
    )

target_link_libraries(
        ruis
        android log z ${ANDROID_GRADLE_NATIVE_MODULES}
    )
