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

#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

TEST_F(EmulateFixture, CbwPositive)
{
    write_reg(AL, 0x40);
    set_instruction({0x98});

    emulate();

    ASSERT_EQ(read_reg(AX), 0x0040);
    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), FLAGS_STUCK_BITS);
}

TEST_F(EmulateFixture, CbwNegative)
{
    write_reg(AL, 0x80);
    set_instruction({0x98});

    emulate();

    ASSERT_EQ(read_reg(AX), 0xff80);
    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), FLAGS_STUCK_BITS);
}

TEST_F(EmulateFixture, CwdPositive)
{
    write_reg(AX, 0x4000);
    set_instruction({0x99});

    emulate();

    ASSERT_EQ(read_reg(AX), 0x4000);
    ASSERT_EQ(read_reg(DX), 0x0000);
    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), FLAGS_STUCK_BITS);
}

TEST_F(EmulateFixture, CwdNegative)
{
    write_reg(AX, 0x8000);
    set_instruction({0x99});

    emulate();

    ASSERT_EQ(read_reg(AX), 0x8000);
    ASSERT_EQ(read_reg(DX), 0xffff);
    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), FLAGS_STUCK_BITS);
}
