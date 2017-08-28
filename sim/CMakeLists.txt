set(BIOS_PLATFORM "sim")
add_subdirectory(../bios ${CMAKE_CURRENT_BINARY_DIR}/bios)

include(Verilator)
include(Nasm)

find_package(Boost COMPONENTS program_options REQUIRED)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIR})

add_subdirectory(common)
add_subdirectory(cppmodel)
add_subdirectory(RTLCPU)
add_subdirectory(display)
include_directories(cppmodel
                    display
                    common
                    RTLCPU
                    ../tests/rtl)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/RTLCPU)

add_executable(simulator
               Simulator.cpp
               CGA.h
               Keyboard.h
               UART.h
               UART.cpp
               SPI.h
               SPI.cpp)
target_link_libraries(simulator
                      simcommon
                      simdisplay
                      8086sim
                      rtlsim
                      ${Boost_LIBRARIES})
install(TARGETS simulator
        COMPONENT simulator
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        RESOURCE DESTINATION share/s80x86)
