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

add_library(instructionsRTL OBJECT
            TestStringInterrupt.cpp)
add_library(instructionsnohw OBJECT
            TestIO.cpp)
add_library(instructions OBJECT
            Arithmetic.cpp
            Shift.cpp
            TestAdc.cpp
            TestAdd.cpp
            TestAnd.cpp
            TestAscii.cpp
            TestBound.cpp
            TestCall.cpp
            TestCmp.cpp
            TestControl.cpp
            TestDiv.cpp
            TestEnter.cpp
            TestExtend.cpp
            TestFlags.cpp
            TestJump.cpp
            TestIncDec.cpp
            TestInt.cpp
            TestInvalid.cpp
            TestLeave.cpp
            TestLock.cpp
            TestLoop.cpp
            TestMisc.cpp
            TestMov.cpp
            TestMul.cpp
            TestNeg.cpp
            TestNot.cpp
            TestOr.cpp
            TestPointer.cpp
            TestPush.cpp
            TestRcl.cpp
            TestRcr.cpp
            TestRet.cpp
            TestRol.cpp
            TestRor.cpp
            TestSar.cpp
            TestSegment.cpp
            TestSbb.cpp
            TestShl.cpp
            TestShr.cpp
            TestString.cpp
            TestSub.cpp
            TestTest.cpp
            TestTrap.cpp
            TestXchg.cpp
            TestXlat.cpp
            TestXor.cpp)
