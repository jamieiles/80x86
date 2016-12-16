include(Verilator)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../rtl)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/coverage)

add_executable(rtl-unittest
               TestFifo.cpp
               TestPrefetch.cpp
               TestRegisterFile.cpp
               TestSegmentRegisterFile.cpp
               main.cpp)

target_link_libraries(rtl-unittest
                      VFifo
                      VPrefetch
                      VRegisterFile
                      VSegmentRegisterFile
		      gtest
		      gmock)
add_test(rtl-unittest rtl-unittest)
