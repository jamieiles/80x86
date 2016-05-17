cmake_minimum_required(VERSION 2.8)
project(8086 CXX)

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
