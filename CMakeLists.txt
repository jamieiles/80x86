cmake_minimum_required(VERSION 2.8)
project(8086 CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/")

find_package(Nasm)
find_package(Verilator)
find_program(CCACHE_FOUND ccache)
find_package(Asciidoctor REQUIRED)

if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_LINK_DEPENDS_NO_SHARED 1)
set(CMAKE_CONFIGURATION_TYPES "Coverage;Release;Debug")

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -ggdb3 -std=c++14")
set(CMAKE_CXX_FLAGS_COVERAGE "-O0 -fprofile-arcs -ftest-coverage -fno-elide-constructors")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG ${CMAKE_CXX_FLAGS_DEBUG}")

enable_testing()

add_subdirectory(googletest)
add_subdirectory(python)
add_subdirectory(sim)
add_subdirectory(tests)
add_subdirectory(rtl)
add_subdirectory(documentation)

include(CTest)
