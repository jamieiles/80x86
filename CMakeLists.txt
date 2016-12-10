cmake_minimum_required(VERSION 2.8)
project(8086 CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/")

find_package(Nasm)
find_package(Verilator)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_LINK_DEPENDS_NO_SHARED 1)
set(CMAKE_CONFIGURATION_TYPES "Coverage;Release")

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -ggdb3 -std=c++14")
set(CMAKE_CXX_FLAGS_COVERAGE "-O0 -fprofile-arcs -ftest-coverage -fno-elide-constructors")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

enable_testing()

add_subdirectory(googletest)
add_subdirectory(python)
add_subdirectory(sim)
add_subdirectory(tests)
add_subdirectory(rtl)

include(CTest)
