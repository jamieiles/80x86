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

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/coverage)

macro(configure_test test_file)
    configure_file(${test_file} ${CMAKE_CURRENT_BINARY_DIR}/${test_file})
endmacro(configure_test)

macro(system_test test_file sim)
    add_test(${sim}_${test_file} python ${CMAKE_CURRENT_BINARY_DIR}/${test_file} ${sim})
    set_tests_properties(${sim}_${test_file} PROPERTIES TIMEOUT 30)
    set_property(TEST ${sim}_${test_file} PROPERTY ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/..:${CMAKE_CURRENT_BINARY_DIR}/../../python")
    if (${sim} MATCHES "^JTAGCPU$")
        set_tests_properties(${sim}_${test_file} PROPERTIES RESOURCE_LOCK "JTAG")
    endif()
    set_tests_properties(${sim}_${test_file} PROPERTIES LABELS ${sim})
endmacro(system_test)

configure_test(moves.py)
configure_test(calls_and_jumps.py)
configure_test(modrm_override.py)
configure_test(irq.py)
configure_test(singlestep.py)
system_test(moves.py Sim)
system_test(moves.py RTLCPU)
system_test(moves.py JTAGCPU)
system_test(calls_and_jumps.py Sim)
system_test(calls_and_jumps.py RTLCPU)
system_test(calls_and_jumps.py JTAGCPU)
system_test(modrm_override.py Sim)
system_test(modrm_override.py RTLCPU)
system_test(modrm_override.py JTAGCPU)
system_test(singlestep.py Sim)
system_test(singlestep.py RTLCPU)
system_test(singlestep.py JTAGCPU)
system_test(irq.py JTAGCPU)
