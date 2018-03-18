# Copyright Jamie Iles, 2017
#
# This file is part of s80x86.
#
# s80x86 is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# s80x86 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with s80x86.  If not, see <http://www.gnu.org/licenses/>.

include(Verilator)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../rtl/microcode)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../rtl)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../rtl)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../tests/rtl)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/../../rtl/microcode/Microcode.sv PROPERTIES
                            COMPILE_FLAGS "-DMICROCODE_ROM_PATH=\\\"${CMAKE_CURRENT_BINARY_DIR}/../../rtl/microcode/\\\"")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/../../fpga/common/Cache.sv PROPERTIES
                            COMPILE_FLAGS "-DCACHE_SIZE=${CACHE_SIZE}")

get_property(ALU_SOURCES GLOBAL PROPERTY G_ALU_SOURCES)
get_property(CORE_SOURCES GLOBAL PROPERTY G_CORE_SOURCES)

verilate(TOPLEVEL RTLCPU
         VERILOG_SOURCES
         ${CMAKE_CURRENT_BINARY_DIR}/../../config.v
         ${CMAKE_CURRENT_SOURCE_DIR}/RTLCPU.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/MemArbiter.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../fpga/common/Cache.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../fpga/common/BlockRam.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../fpga/common/DPRam.sv
         ${CORE_SOURCES}
         GENERATED_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/VRTLCPU___024unit.cpp
         ${CMAKE_CURRENT_BINARY_DIR}/VRTLCPU___024unit.h
         DEPENDS generate_microcode generate_instruction_definitions)

add_library(rtlsim SHARED
            Core.cpp)

target_link_libraries(rtlsim
                      simcommon
                      verilator
                      VRTLCPU)
set_target_properties(rtlsim PROPERTIES LINK_FLAGS "-Wl,--export-dynamic")

install(TARGETS VRTLCPU
        LIBRARY DESTINATION lib
        COMPONENT simulator)
