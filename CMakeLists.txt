cmake_minimum_required(VERSION 2.8)
project(8086 CXX)

set(CMAKE_CXX_FLAGS "-Wall -Werror -O2 -ggdb3 -std=c++14")

enable_testing()

add_subdirectory(googletest)
add_subdirectory(sim)
add_subdirectory(tests)
