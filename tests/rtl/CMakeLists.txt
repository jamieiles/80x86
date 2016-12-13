include(Verilator)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../rtl)

add_executable(rtl-unittest
               fifo.cpp
               prefetch.cpp
               main.cpp)

target_link_libraries(rtl-unittest
                      VFifo
                      VPrefetch
		      gtest
		      gmock)
add_test(rtl-unittest rtl-unittest)
