set(BIOS_PLATFORM "sim")
add_subdirectory(../bios ${CMAKE_CURRENT_BINARY_DIR}/bios)

include(Verilator)
include(Nasm)

find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_subdirectory(common)
add_subdirectory(cppmodel)
add_subdirectory(RTLCPU)
include_directories(cppmodel
                    common
                    RTLCPU
                    ../tests/rtl)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/RTLCPU)

add_executable(simulator
               Simulator.cpp
               UART.h
               UART.cpp
               SPI.h
               SPI.cpp)
target_link_libraries(simulator
                      simcommon
                      8086sim
                      rtlsim
                      ${Boost_LIBRARIES})
install(TARGETS simulator
        COMPONENT simulator
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        RESOURCE DESTINATION share/s80x86)
