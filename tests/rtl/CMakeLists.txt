include(Verilator)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../rtl)

add_executable(rtl-unittest
               fifo.cpp)

target_link_libraries(rtl-unittest
                      VFifo
		      gtest
		      gmock)
add_test(rtl-unittest rtl-unittest)
