include(Verilator)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../rtl)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../rtl)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/coverage)

verilate(ModRMTestbench ${CMAKE_CURRENT_SOURCE_DIR}/ModRMTestbench.v
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/ModRMDecode.v
         ${CMAKE_CURRENT_SOURCE_DIR}/../../rtl/ImmediateReader.v)

add_library(rtlsim SHARED
            Core.cpp
            ../../sim/cpu/RegisterFile.cpp
            ../../sim/cpu/Memory.cpp)
target_link_libraries(rtlsim
                      verilator
                      VCore)
set_target_properties(rtlsim PROPERTIES LINK_FLAGS "-Wl,--export-dynamic")

add_executable(rtl-unittest
               TestALU.cpp
               TestCSIPSync.cpp
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
               TestSegmentRegisterFile.cpp
	       $<TARGET_OBJECTS:instructions>
               main.cpp)

target_link_libraries(rtl-unittest
                      VALU
                      VCSIPSync
                      VCore
                      VFifo
                      VFlags
                      VImmediateReader
                      VIP
                      VLoadStore
                      VMicrocode
                      VModRMTestbench
                      VPrefetch
                      VRegisterFile
                      VSegmentRegisterFile
		      gtest
		      gmock
                      verilator
                      rtlsim)
add_test(rtl-unittest rtl-unittest)
set_tests_properties(rtl-unittest PROPERTIES TIMEOUT 5)

install(TARGETS rtl-unittest rtlsim
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
