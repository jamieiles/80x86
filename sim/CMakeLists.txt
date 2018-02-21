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

set(CACHE_SIZE 8192)

set(BIOS_PLATFORM "sim")
add_definitions(-DSERIAL_STDIO)
add_subdirectory(../bios ${CMAKE_CURRENT_BINARY_DIR}/bios)

include(Verilator)
include(Nasm)

find_package(Boost COMPONENTS program_options serialization REQUIRED)
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
               Mouse.h
               PS2.h
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
