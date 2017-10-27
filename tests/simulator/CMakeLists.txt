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

include_directories(..)
include_directories(../../sim/cppmodel)

add_library(simtests OBJECT
	    TestFifo.cpp
	    TestMemory.cpp
	    TestModRM.cpp
	    TestRegisterFile.cpp)

add_executable(sim-unittest
               SimMain.cpp
	       $<TARGET_OBJECTS:instructions>
	       $<TARGET_OBJECTS:instructionsnohw>
	       $<TARGET_OBJECTS:simtests>)
target_link_libraries(sim-unittest
		      8086sim
		      gtest
		      gmock)

add_test(sim-unittest ./sim-unittest --gtest_color=yes)
set_tests_properties(sim-unittest PROPERTIES TIMEOUT 5)
set_tests_properties(sim-unittest PROPERTIES LABELS SoftwareCPU)
