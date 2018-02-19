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

#include <gtest/gtest.h>
#include "CPU.h"
#include "EmulateFixture.h"

#include <iostream>

TEST_F(EmulateFixture, TrapOnInt3)
{
    set_instruction({0x90});
    write_mem16(VEC_INT + 2, 0x8000, CS);
    write_mem16(VEC_INT + 0, 0x0100, CS);
    emulate();

    ASSERT_FALSE(cpu->has_trapped());

    reset();

    set_instruction({0xcc});
    emulate();

    EXPECT_TRUE(cpu->has_trapped());
    EXPECT_EQ(0x8000, cpu->read_reg(CS));
    EXPECT_EQ(0x0100, cpu->read_reg(IP));
}
