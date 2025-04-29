cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

project(test)

# TODO: are these needed here explicitly?
find_package(utki CONFIG REQUIRED)
find_package(papki CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Freetype CONFIG REQUIRED)

find_package(ruis CONFIG REQUIRED)

add_executable(test main.cpp)

target_link_libraries(test PRIVATE ruis::ruis)
