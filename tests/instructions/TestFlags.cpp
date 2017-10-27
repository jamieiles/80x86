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

TEST_F(EmulateFixture, Lahf)
{
    set_instruction({0x9f});
    write_flags(0xd7);

    emulate();

    ASSERT_EQ(0xd7, read_reg(AH));
}

TEST_F(EmulateFixture, Sahf)
{
    set_instruction({0x9e});
    uint16_t old_flags = CF;
    write_flags(old_flags);
    write_reg(AH, CF | PF | AF);

    emulate();

    ASSERT_EQ(CF | PF | AF | FLAGS_STUCK_BITS, read_flags());
}

TEST_F(EmulateFixture, Pushf)
{
    write_flags(0x00d7);
    write_reg(SP, 0x0100);
    set_instruction({0x9c});

    emulate();

    ASSERT_EQ(read_reg(SP), 0x00fe);
    ASSERT_EQ(0xf0d7, read_mem16(0x00fe, SS));
}

TEST_F(EmulateFixture, Popf)
{
    write_reg(SP, 0x00fe);
    write_mem16(0x00fe, 0x00d7, SS);
    set_instruction({0x9d});

    emulate();

    ASSERT_EQ(read_reg(SP), 0x0100);
    ASSERT_EQ(0xf0d7, read_flags());
}

TEST_F(EmulateFixture, Setalc0)
{
    set_instruction({0xd6});

    emulate();

    ASSERT_EQ(0x0, read_reg(AL));
}

TEST_F(EmulateFixture, Setalc1)
{
    write_flags(CF);

    set_instruction({0xd6});

    emulate();

    ASSERT_EQ(0xff, read_reg(AL));
}
