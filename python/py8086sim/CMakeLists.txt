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

include(Verilator)

set(Python_ADDITIONAL_VERSIONS 3.8)
find_package(PythonLibs 3.8 REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
find_package(Boost 1.71.0 COMPONENTS python38 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(../../sim/cppmodel)
include_directories(../../sim/RTLCPU)
include_directories(../../sim/common)
include_directories(../../tests/rtl)
include_directories(../../tests/jtag)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../tests/rtl)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../rtl)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../sim/RTLCPU)

add_library(Cpu SHARED
            Cpu.cpp)
set_target_properties(Cpu PROPERTIES PREFIX "")
target_link_libraries(Cpu 8086sim rtlsim JTAGCPU ${Boost_LIBRARIES})

file(COPY __init__.py
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
