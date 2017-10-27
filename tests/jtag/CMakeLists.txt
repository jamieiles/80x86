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

find_package(libusb-1.0 REQUIRED)

add_library(JTAGCPU SHARED
            JTAGCPU.cpp JTAGCPU.h
            ${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/altera-jtag/jtag.c
            ${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/altera-jtag/jtag-virtual.c)
include_directories(${LIBUSB_1_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/altera-jtag)
target_link_libraries(JTAGCPU ${LIBUSB_1_LIBRARIES})

add_executable(jtag-unittest
               $<TARGET_OBJECTS:instructions>
               main.cpp)
target_link_libraries(jtag-unittest
                      gtest
                      gmock
                      JTAGCPU)

add_test(jtag-unittest jtag-unittest)
set_tests_properties(jtag-unittest PROPERTIES TIMEOUT 1200)
set_tests_properties(jtag-unittest PROPERTIES RESOURCE_LOCK "JTAG")
set_tests_properties(jtag-unittest PROPERTIES LABELS JTAG)
