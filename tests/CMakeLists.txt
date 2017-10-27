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

include_directories(../vendor/googletest/googletest/include)
include_directories(../vendor/googletest/googlemock/include)
include_directories(../sim/common)
include_directories(../sim/cppmodel)
include_directories(../sim)
include_directories(include)

add_subdirectory(programs)
add_subdirectory(instructions)
add_subdirectory(integration)
add_subdirectory(simulator)
add_subdirectory(rtl)
add_subdirectory(jtag)

configure_file(python/Runner.py ${CMAKE_CURRENT_BINARY_DIR}/Runner.py)
