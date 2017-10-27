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
#include "Flags.h"

TEST_F(EmulateFixture, RetIntra)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    write_reg(SP, 0x00fe);
    write_mem16(0x00fe, 0x0100, SS);

    set_instruction({0xc3});

    emulate();

    ASSERT_EQ(0x0100, read_reg(SP));
    ASSERT_EQ(0x2000, read_reg(CS));
    ASSERT_EQ(0x0100, read_reg(IP));
}

TEST_F(EmulateFixture, RetIntraAddSp)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    write_reg(SP, 0x00fe);
    write_mem16(0x00fe, 0x0100, SS);

    set_instruction({0xc2, 0x10, 0x00});

    emulate();

    ASSERT_EQ(0x0110, read_reg(SP));
    ASSERT_EQ(0x2000, read_reg(CS));
    ASSERT_EQ(0x0100, read_reg(IP));
}

TEST_F(EmulateFixture, RetInter)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    write_reg(SP, 0x00fc);
    write_mem16(0x00fe, 0x8000, SS);
    write_mem16(0x00fc, 0x0100, SS);

    set_instruction({0xcb});

    emulate();

    ASSERT_EQ(0x0100, read_reg(SP));
    ASSERT_EQ(0x8000, read_reg(CS));
    ASSERT_EQ(0x0100, read_reg(IP));
}

TEST_F(EmulateFixture, RetInterAddSp)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    write_reg(SP, 0x00fc);
    write_mem16(0x00fe, 0x8000, SS);
    write_mem16(0x00fc, 0x0100, SS);

    set_instruction({0xca, 0x10, 0x00});

    emulate();

    ASSERT_EQ(0x0110, read_reg(SP));
    ASSERT_EQ(0x8000, read_reg(CS));
    ASSERT_EQ(0x0100, read_reg(IP));
}

TEST_F(EmulateFixture, Iret)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    write_reg(SP, 0x00fa);
    write_mem16(0x00fe, FLAGS_STUCK_BITS | CF, SS);
    write_mem16(0x00fc, 0x8000, SS);
    write_mem16(0x00fa, 0x0100, SS);

    set_instruction({0xcf});

    emulate();

    ASSERT_EQ(0x0100, read_reg(SP));
    ASSERT_EQ(0x8000, read_reg(CS));
    ASSERT_EQ(0x0100, read_reg(IP));
    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), FLAGS_STUCK_BITS | CF);
}
