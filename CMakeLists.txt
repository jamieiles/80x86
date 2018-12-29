cmake_minimum_required(VERSION 2.8)
project(s80x86 C CXX ASM)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_NAME "s80x86")
set(CPACK_PACKAGE_CONTACT "jamie@jamieiles.com")
set(CPACK_STRIP_FILES ON)
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_ALL simulator)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-python1.58.0, python3, libsdl2-2.0.0")
include(CPack)
include(InstallRequiredSystemLibraries)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/")

include(MacroBoolTo01)
find_package(Nasm)
find_package(Verilator)
find_program(CCACHE_FOUND ccache)
find_package(Asciidoctor REQUIRED)
find_package(Quartus REQUIRED)

if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_LINK_DEPENDS_NO_SHARED 1)
set(CMAKE_CONFIGURATION_TYPES "Coverage;Release;Debug")

set(CMAKE_CXX_FLAGS "-Weffc++ -march=native -Wall -Wextra -Werror -ggdb3 -gsplit-dwarf -Qunused-arguments -std=c++14 ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_COVERAGE "-O0 -fprofile-arcs -ftest-coverage -fno-elide-constructors ${CMAKE_CXX_FLAGS_COVERAGE} -DCOVERAGE")
set(CMAKE_EXE_LINKER_FLAGS_COVERAGE "--coverage -Wl,--gdb-index")
set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE "--coverage -Wl,--gdb-index")
set(CMAKE_CXX_FLAGS_RELEASE "-Os ${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -ggdb3 -O0 ${CMAKE_CXX_FLAGS_DEBUG}")

set(POISON_MEMORY_VALUE "0x00" CACHE STRING "Poison all memory with this value in tests")
add_definitions(-DMEM_INIT_BYTE_VAL=${POISON_MEMORY_VALUE})

option(BUILD_DE0_CV "Build an image for the DE0-CV board" OFF)
option(BUILD_DE0_NANO "Build an image for the DE0-nano board" OFF)
option(BUILD_HDP001 "Build an image for the HDP001 board" OFF)
option(S80X86_TRAP_ESCAPE "Trap on ESC opcodes" OFF)
option(S80X86_PSEUDO_286 "Clear bits 12-15 of the flags register to act like 286 real mode" OFF)

macro_bool_to_01(S80X86_PSEUDO_286 S80X86_PSEUDO_286_INT)

configure_file(config.h.in config.h)
configure_file(config.v.in config.v)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

enable_testing()

add_subdirectory(vendor/googletest)
add_subdirectory(rtl)
add_subdirectory(python)
add_subdirectory(sim)
add_subdirectory(tests)
add_subdirectory(documentation)

if(${BUILD_DE0_CV})
add_subdirectory(fpga/de0-cv)
endif()

if(${BUILD_DE0_NANO})
add_subdirectory(fpga/de0-nano)
endif()

if(${BUILD_HDP001})
add_subdirectory(fpga/hdp001)
endif()

include(CTest)
