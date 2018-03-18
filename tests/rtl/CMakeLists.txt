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
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../sim/RTLCPU)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../sim/RTLCPU)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/coverage)

get_property(ALU_SOURCES GLOBAL PROPERTY G_ALU_SOURCES)
get_property(CORE_SOURCES GLOBAL PROPERTY G_CORE_SOURCES)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/../../rtl/microcode/Microcode.sv PROPERTIES
                            COMPILE_FLAGS "-DMICROCODE_ROM_PATH=\\\"${CMAKE_CURRENT_BINARY_DIR}/../../rtl/microcode/\\\"")

verilate(TOPLEVEL ModRMTestbench
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/RegisterEnum.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/ModRMTestbench.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/ModRMDecode.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/ImmediateReader.sv)
verilate(TOPLEVEL DecodeTestbench
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/RegisterEnum.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/Instruction.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/InsnDecoder.sv
         ${CMAKE_CURRENT_BINARY_DIR}/../../rtl/InstructionDefinitions.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/DecodeTestbench.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/ModRMDecode.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/ImmediateReader.sv
         DEPENDS generate_instruction_definitions)
verilate(TOPLEVEL ALU
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/RegisterEnum.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/Instruction.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/InsnDecoder.sv
         ${CMAKE_CURRENT_BINARY_DIR}/../../rtl/microcode/MicrocodeTypes.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/FlagsEnum.sv
         ${ALU_SOURCES}
         GENERATED_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/VALU___024unit.cpp
         ${CMAKE_CURRENT_BINARY_DIR}/VALU___024unit.h
         DEPENDS generate_microcode)
verilate(TOPLEVEL CSIPSync
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/CSIPSync.sv)
verilate(TOPLEVEL Fifo
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/Fifo.sv)
verilate(TOPLEVEL Flags
         VERILOG_SOURCES
         ${CMAKE_CURRENT_BINARY_DIR}/../../config.v
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/FlagsEnum.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/RegisterEnum.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/Instruction.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/InsnDecoder.sv
         ${CMAKE_CURRENT_BINARY_DIR}/../../rtl/microcode/MicrocodeTypes.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/Flags.sv
         DEPENDS generate_microcode)
verilate(TOPLEVEL ImmediateReader
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/ImmediateReader.sv)
verilate(TOPLEVEL IP
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/IP.sv)
verilate(TOPLEVEL LoadStore
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/LoadStore.sv)
verilate(TOPLEVEL Cache
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../fpga/common/Cache.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../fpga/common/BlockRam.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../fpga/common/DPRam.sv)
verilate(TOPLEVEL ModRMDecode
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/RegisterEnum.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/ModRMDecode.sv)
verilate(TOPLEVEL Prefetch
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/Prefetch.sv)
verilate(TOPLEVEL RegisterFile
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/RegisterEnum.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/RegisterFile.sv)
verilate(TOPLEVEL SegmentOverride
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/RegisterEnum.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/SegmentOverride.sv)
verilate(TOPLEVEL SegmentRegisterFile
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/RegisterEnum.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/SegmentRegisterFile.sv)
verilate(TOPLEVEL Divider
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/Divider.sv)
verilate(TOPLEVEL PosedgeToPulse
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/PosedgeToPulse.sv)
verilate(TOPLEVEL PIC
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../fpga/pic/PIC.sv)

add_executable(rtl-unittest
               TestALU.cpp
               TestCache.cpp
               TestCSIPSync.cpp
               TestDecode.cpp
               TestDivider.cpp
               TestDivisionAlgorithm.cpp
               TestFifo.cpp
               TestFlags.cpp
               TestImmediateReader.cpp
               TestImmediateReader.cpp
               TestIP.cpp
               TestLoadStore.cpp
               TestModRMDecode.cpp
               TestModRMDecode.cpp
               TestPIC.cpp
               TestPosedgeToPulse.cpp
               TestPrefetch.cpp
               TestRegisterFile.cpp
               TestSegmentOverride.cpp
               TestSegmentRegisterFile.cpp
	       $<TARGET_OBJECTS:instructions>
	       $<TARGET_OBJECTS:instructionsnohw>
	       $<TARGET_OBJECTS:instructionsRTL>
               main.cpp)

target_link_libraries(rtl-unittest
                      VALU
                      VCache
                      VCSIPSync
                      VRTLCPU
                      VDecodeTestbench
                      VDivider
                      VFifo
                      VFlags
                      VImmediateReader
                      VIP
                      VLoadStore
                      VMicrocode
                      VModRMTestbench
                      VPIC
                      VPosedgeToPulse
                      VPrefetch
                      VRegisterFile
                      VSegmentOverride
                      VSegmentRegisterFile
		      gtest
		      gmock
                      verilator
                      rtlsim
                      simcommon)
add_test(rtl-unittest rtl-unittest)
set_tests_properties(rtl-unittest PROPERTIES TIMEOUT 60)
set_tests_properties(rtl-unittest PROPERTIES LABELS RTLCPU)
