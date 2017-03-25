cmake_minimum_required(VERSION 2.8)
project(s80x86 CXX)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_NAME "s80x86")
set(CPACK_PACKAGE_CONTACT "jamie@jamieiles.com")
set(CPACK_STRIP_FILES ON)
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_ALL simulator)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsdl2-2.0-0, libboost-python1.58.0, python3")
include(CPack)
include(InstallRequiredSystemLibraries)

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

set(CMAKE_CXX_FLAGS "-Weffc++ -Wall -Wextra -Werror -ggdb3 -std=c++14 ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_COVERAGE "-O0 -fprofile-arcs -ftest-coverage -fno-elide-constructors ${CMAKE_CXX_FLAGS_COVERAGE}")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 ${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG ${CMAKE_CXX_FLAGS_DEBUG}")

enable_testing()

add_subdirectory(googletest)
add_subdirectory(python)
add_subdirectory(sim)
add_subdirectory(tests)
add_subdirectory(rtl)
add_subdirectory(documentation)

include(CTest)
