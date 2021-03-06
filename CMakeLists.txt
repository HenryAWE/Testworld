# The Main CMakeLists.txt of the Project Testworld
#
# Author: HenryAWE
# License: The 3-clause BSD License


cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/;${CMAKE_MODULE_PATH}")
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    message(STATUS "Use vcpkg toolchain from $ENV{VCPKG_ROOT}")
endif()

# Compiler options
# Use C++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Force UTF-8 on MSVC
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
# Make sure macro "__cplusplus" is correctly defined on MSVC
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>")

project(
    "Testworld"
    VERSION 0.1.0
    DESCRIPTION "A C++/OpenGL project for practicing programing"
    LANGUAGES C CXX
)

add_subdirectory(deps)
add_subdirectory(lang)
add_subdirectory(resource)
add_subdirectory(script)
add_subdirectory(shader)
add_subdirectory(src)
