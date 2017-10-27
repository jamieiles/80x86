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

#include "EmulateFixture.h"

TEST_F(EmulateFixture, LeaAxBx)
{
    // lea ax, [bx]
    set_instruction({0x8d, 0x07});
    write_reg(BX, 0x1234);

    emulate();

    ASSERT_EQ(0x1234, read_reg(AX));
}

TEST_F(EmulateFixture, LeaAxBxSiDisplacement)
{
    // lea ax, [bx+si+0x1234]
    set_instruction({0x8d, 0x80, 0x34, 0x12});
    write_reg(BX, 0x0400);
    write_reg(SI, 0x0100);

    emulate();

    ASSERT_EQ(0x1234 + 0x0400 + 0x0100, read_reg(AX));
}

TEST_F(EmulateFixture, Lds)
{
    // lds si, [0x0100]
    set_instruction({0xc5, 0x36, 0x00, 0x01});
    write_mem32(0x0100, 0x80001234);
    emulate();

    ASSERT_EQ(0x8000, read_reg(DS));
    ASSERT_EQ(0x1234, read_reg(SI));
}

TEST_F(EmulateFixture, Les)
{
    // les si, [0x0100]
    set_instruction({0xc4, 0x36, 0x00, 0x01});
    write_mem32(0x0100, 0x80001234);
    emulate();

    ASSERT_EQ(0x8000, read_reg(ES));
    ASSERT_EQ(0x1234, read_reg(SI));
}
