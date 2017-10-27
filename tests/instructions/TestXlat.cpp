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

TEST_F(EmulateFixture, Xlat)
{
    write_mem8(0x1000 + 0xaa, 0x55);
    write_reg(BX, 0x1000);
    write_reg(AL, 0xaa);
    set_instruction({0xd7});

    emulate();

    ASSERT_EQ(0x55, read_reg(AL));
}
