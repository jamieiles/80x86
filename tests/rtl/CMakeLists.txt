include(Verilator)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../rtl)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../rtl)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/coverage)

verilate(ModRMTestbench ${CMAKE_CURRENT_SOURCE_DIR}/ModRMTestbench.v
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/ModRMDecode.v
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/ImmediateReader.v)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/microcode.bin
                          ${CMAKE_CURRENT_BINARY_DIR}/Microcode.v
                          ${CMAKE_CURRENT_BINARY_DIR}/MicrocodeTypes.v
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/microassembler/uasm
                           ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/microcode/microcode.us
                           ${CMAKE_CURRENT_BINARY_DIR}/microcode.bin
                           ${CMAKE_CURRENT_BINARY_DIR}/Microcode.v
                           ${CMAKE_CURRENT_BINARY_DIR}/MicrocodeTypes.v
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/microcode/microcode.us
                           ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/microcode/Microcode.v.templ
                           ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/microcode/MicrocodeTypes.v.templ)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/Microcode.v PROPERTIES
                            COMPILE_FLAGS "-DMICROCODE_ROM_PATH=\\\"${CMAKE_CURRENT_BINARY_DIR}\\\"")
verilate(Microcode ${CMAKE_CURRENT_BINARY_DIR}/Microcode.v)

add_executable(rtl-unittest
               TestFifo.cpp
               TestImmediateReader.cpp
               TestImmediateReader.cpp
               TestLoadStore.cpp
               TestMicrocode.cpp
               TestModRMDecode.cpp
               TestModRMDecode.cpp
               TestPrefetch.cpp
               TestRegisterFile.cpp
               TestSegmentRegisterFile.cpp
               ../../sim/cpu/RegisterFile.cpp
               main.cpp)

target_link_libraries(rtl-unittest
                      VFifo
                      VImmediateReader
                      VLoadStore
                      VMicrocode
                      VModRMTestbench
                      VPrefetch
                      VRegisterFile
                      VSegmentRegisterFile
		      gtest
		      gmock
                      verilator)
add_test(rtl-unittest rtl-unittest)
