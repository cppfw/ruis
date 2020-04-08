# Sets the minimum version of CMake required to build your native library.
# This ensures that a certain set of CMake features is available to
# your build.

cmake_minimum_required(VERSION 3.4.1)

# Specifies a library name, specifies whether the library is STATIC or
# SHARED, and provides relative paths to the source code. You can
# define multiple libraries by adding multiple add_library() commands,
# and CMake builds them for you. When you build your app, Gradle
# automatically packages shared libraries with your APK.

file(GLOB_RECURSE srcs "../../src/morda/morda/*.cpp")

link_directories( build/ndkLibs/${ANDROID_ABI}/ )

add_library( # Specifies the name of the library.
             morda

             # Sets the library as a shared library.
             STATIC

             # Provides a relative path to your source file(s).
             ${srcs} )

include_directories(
        build/ndkLibs/include/
        build/ndkLibs/include/freetype
        build/ndkLibs/include/png
        build/ndkLibs/include/jpeg
    )

target_link_libraries(
        morda
        android log png jpeg puu papki svgren svgdom z freetype mikroxml unikod utki cairo pixman
    )
