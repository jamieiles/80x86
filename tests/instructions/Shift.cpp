// Copyright Jamie Iles, 2017
//
// This file is part of s80x86.
//
// s80x86 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// s80x86 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with s80x86.  If not, see <http://www.gnu.org/licenses/>.

#include "Shift.h"
#include "EmulateFixture.h"
#include "Flags.h"

TEST_P(ShiftReg8Test, ResultAndFlags)
{
    for (auto &t : GetParam().second) {
        reset();

        write_flags(t.flags);
        write_reg(CL, t.shift_count);
        SCOPED_TRACE("SHIFT " + std::to_string(static_cast<int>(t.val)) +
                     " << " + std::to_string(static_cast<int>(t.shift_count)) +
                     ((t.flags & CF) ? " + CF" : ""));
        write_reg(AL, t.val);
        set_instruction(GetParam().first);

        emulate();

        ASSERT_EQ(read_reg(AL), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_P(ShiftMem8Test, ResultAndFlags)
{
    for (auto &t : GetParam().second) {
        reset();

        write_flags(t.flags);
        write_reg(CL, t.shift_count);
        SCOPED_TRACE("SHIFT " + std::to_string(static_cast<int>(t.val)) +
                     " << " + std::to_string(static_cast<int>(t.shift_count)) +
                     ((t.flags & CF) ? " + CF" : ""));
        write_reg(BX, 0x100);
        write_mem8(0x100, t.val);
        set_instruction(GetParam().first);

        emulate();

        ASSERT_EQ(read_mem8(0x100), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_P(ShiftRegImm8Test, ResultAndFlags)
{
    for (auto &t : GetParam().second) {
        reset();

        write_flags(t.flags);
        SCOPED_TRACE("SHIFT " + std::to_string(static_cast<int>(t.val)) +
                     " << " + std::to_string(static_cast<int>(t.shift_count)) +
                     ((t.flags & CF) ? " + CF" : ""));
        write_reg(AL, t.val);
        auto instr = GetParam().first;
        instr.push_back(static_cast<uint8_t>(t.shift_count));
        set_instruction(instr);

        emulate();

        ASSERT_EQ(read_reg(AL), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_P(ShiftMemImm8Test, ResultAndFlags)
{
    for (auto &t : GetParam().second) {
        reset();

        write_flags(t.flags);
        SCOPED_TRACE("SHIFT " + std::to_string(static_cast<int>(t.val)) +
                     " << " + std::to_string(static_cast<int>(t.shift_count)) +
                     ((t.flags & CF) ? " + CF" : ""));
        write_reg(BX, 0x100);
        write_mem8(0x100, t.val);
        auto instr = GetParam().first;
        instr.push_back(static_cast<uint8_t>(t.shift_count));
        set_instruction(instr);

        emulate();

        ASSERT_EQ(read_mem8(0x100), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_P(ShiftReg16Test, ResultAndFlags)
{
    // shl ax, COUNT
    for (auto &t : GetParam().second) {
        reset();

        write_flags(t.flags);
        write_reg(CL, t.shift_count);
        SCOPED_TRACE("SHIFT " + std::to_string(static_cast<int>(t.val)) +
                     " << " + std::to_string(static_cast<int>(t.shift_count)) +
                     ((t.flags & CF) ? " + CF" : ""));
        write_reg(AX, t.val);
        set_instruction(GetParam().first);

        emulate();

        ASSERT_EQ(read_reg(AX), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_P(ShiftMem16Test, ResultAndFlags)
{
    for (auto &t : GetParam().second) {
        reset();

        write_flags(t.flags);
        write_reg(CL, t.shift_count);
        SCOPED_TRACE("SHIFT " + std::to_string(static_cast<int>(t.val)) +
                     " << " + std::to_string(static_cast<int>(t.shift_count)) +
                     ((t.flags & CF) ? " + CF" : ""));
        write_reg(BX, 0x100);
        write_mem16(0x100, t.val);
        set_instruction(GetParam().first);

        emulate();

        ASSERT_EQ(read_mem16(0x100), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_P(ShiftRegImm16Test, ResultAndFlags)
{
    for (auto &t : GetParam().second) {
        reset();

        write_flags(t.flags);
        SCOPED_TRACE("SHIFT " + std::to_string(static_cast<int>(t.val)) +
                     " << " + std::to_string(static_cast<int>(t.shift_count)) +
                     ((t.flags & CF) ? " + CF" : ""));
        write_reg(AX, t.val);
        auto instr = GetParam().first;
        instr.push_back(t.shift_count & 0xff);
        set_instruction(instr);

        emulate();

        ASSERT_EQ(read_reg(AX), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_P(ShiftMemImm16Test, ResultAndFlags)
{
    for (auto &t : GetParam().second) {
        reset();

        write_flags(t.flags);
        SCOPED_TRACE("SHIFT " + std::to_string(static_cast<int>(t.val)) +
                     " << " + std::to_string(static_cast<int>(t.shift_count)) +
                     ((t.flags & CF) ? " + CF" : ""));
        write_reg(BX, 0x100);
        write_mem16(0x100, t.val);
        auto instr = GetParam().first;
        instr.push_back(t.shift_count & 0xff);
        set_instruction(instr);

        emulate();

        ASSERT_EQ(read_mem16(0x100), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

// Shifting the count is an odd thing to do, but we should shift the initial
// value even though it is decremented on each iteration.
TEST_P(ShiftCLTest, ShiftCL)
{
    write_reg(CL, GetParam().count);
    set_instruction(GetParam().instruction);

    emulate();

    ASSERT_EQ(GetParam().expected, read_reg(CL));
}
