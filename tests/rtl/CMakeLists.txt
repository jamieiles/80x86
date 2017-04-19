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
verilate(TOPLEVEL ALU
         VERILOG_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/../../rtl/microcode/MicrocodeTypes.sv
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
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/FlagsEnum.sv
         ${CMAKE_CURRENT_BINARY_DIR}/../../rtl/microcode/MicrocodeTypes.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/Flags.sv
         DEPENDS generate_microcode)
verilate(TOPLEVEL ImmediateReader
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/ImmediateReader.sv)
verilate(TOPLEVEL IP
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/IP.sv)
verilate(TOPLEVEL LoadStore
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/LoadStore.sv)
verilate(TOPLEVEL ModRMDecode
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/RegisterEnum.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/ModRMDecode.sv)
verilate(TOPLEVEL Prefetch
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/Prefetch.sv)
verilate(TOPLEVEL RegisterFile
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/RegisterFile.sv)
verilate(TOPLEVEL SegmentOverride
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/RegisterEnum.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/SegmentOverride.sv)
verilate(TOPLEVEL SegmentRegisterFile
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/RegisterEnum.sv
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/SegmentRegisterFile.sv)
verilate(TOPLEVEL Divider
         VERILOG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/Divider.sv)

add_executable(rtl-unittest
               TestALU.cpp
               TestCSIPSync.cpp
               TestDivider.cpp
               TestDivisionAlgorithm.cpp
               TestFifo.cpp
               TestFlags.cpp
               TestImmediateReader.cpp
               TestImmediateReader.cpp
               TestIP.cpp
               TestLoadStore.cpp
               TestMicrocode.cpp
               TestModRMDecode.cpp
               TestModRMDecode.cpp
               TestPrefetch.cpp
               TestRegisterFile.cpp
               TestSegmentOverride.cpp
               TestSegmentRegisterFile.cpp
	       $<TARGET_OBJECTS:instructions>
	       $<TARGET_OBJECTS:instructionsnohw>
               main.cpp)

target_link_libraries(rtl-unittest
                      VALU
                      VCSIPSync
                      VRTLCPU
                      VDivider
                      VFifo
                      VFlags
                      VImmediateReader
                      VIP
                      VLoadStore
                      VMicrocode
                      VModRMTestbench
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
