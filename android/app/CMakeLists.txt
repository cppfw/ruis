# Sets the minimum version of CMake required to build your native library.
# This ensures that a certain set of CMake features is available to
# your build.

cmake_minimum_required(VERSION 3.4.1)

# Specifies a library name, specifies whether the library is STATIC or
# SHARED, and provides relative paths to the source code. You can
# define multiple libraries by adding multiple add_library() commands,
# and CMake builds them for you. When you build your app, Gradle
# automatically packages shared libraries with your APK.

file(GLOB_RECURSE srcs
        "../../tests/app/src/*.cpp"
        "../../tests/harness/mordavokne/*.cpp"
        "../../tests/harness/opengles2/*.cpp"
    )

include (${ANDROID_GRADLE_NATIVE_BUNDLE_PLUGIN_MK})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

link_directories( ../morda/build/intermediates/cmake/release/obj/${ANDROID_ABI} )

add_library(
        # Specifies the name of the library.
        mordavokneapp # this has to be 'mordavokneapp' so that MordaVOkneActivity loads that named .so

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${srcs}
    )

include_directories(
        ../../src/
        ../../tests/harness/mordavokne/
        ../../tests/harness/opengles2/
    )

message(LINK = ${ANDROID_GRADLE_NATIVE_MODULES})

target_link_libraries(
        mordavokneapp
        morda android log EGL z GLESv2 ${ANDROID_GRADLE_NATIVE_MODULES}
    )
