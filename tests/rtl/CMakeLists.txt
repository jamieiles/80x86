include(Verilator)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../rtl)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../rtl)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/coverage)

verilate(ModRMTestbench ${CMAKE_CURRENT_SOURCE_DIR}/ModRMTestbench.v
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/ModRMDecode.v
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/ImmediateReader.v)

add_executable(rtl-unittest
               Core.cpp
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
               ../../sim/cpu/Memory.cpp
               main.cpp)

target_link_libraries(rtl-unittest
                      VCore
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
set_tests_properties(rtl-unittest PROPERTIES TIMEOUT 5)
