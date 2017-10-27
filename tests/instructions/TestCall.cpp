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

TEST_F(EmulateFixture, CallDirectIntra)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    write_reg(SP, 0x0100);
    set_instruction({0xe8, 0x10, 0x20});

    emulate();

    ASSERT_EQ(0x00fe, read_reg(SP));
    ASSERT_EQ(0x0033, read_mem16(0x00fe, SS));
    ASSERT_EQ(0x2000, read_reg(CS));
    ASSERT_EQ(0x2043, read_reg(IP));
}

TEST_F(EmulateFixture, CallIndirectIntraReg)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    write_reg(SP, 0x0100);

    write_reg(BX, 0x2010);

    // call near bx
    set_instruction({0xff, 0xd3});

    emulate();

    ASSERT_EQ(0x00fe, read_reg(SP));
    ASSERT_EQ(0x0032, read_mem16(0x00fe, SS));
    ASSERT_EQ(0x2000, read_reg(CS));
    ASSERT_EQ(0x2010, read_reg(IP));
}

TEST_F(EmulateFixture, CallIndirectIntraMem)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    write_reg(SP, 0x0100);

    write_reg(BX, 0x0200);
    write_mem16(0x0200, 0x2010);

    // call near [bx]
    set_instruction({0xff, 0x17});

    emulate();

    ASSERT_EQ(0x00fe, read_reg(SP));
    ASSERT_EQ(0x0032, read_mem16(0x00fe, SS));
    ASSERT_EQ(0x2000, read_reg(CS));
    ASSERT_EQ(0x2010, read_reg(IP));
}

TEST_F(EmulateFixture, CallDirectInter)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    write_reg(SP, 0x0100);
    set_instruction({0x9a, 0x10, 0x20, 0x00, 0x80});

    emulate();

    ASSERT_EQ(0x00fc, read_reg(SP));
    ASSERT_EQ(0x2000, read_mem16(0x00fe, SS));
    ASSERT_EQ(0x0035, read_mem16(0x00fc, SS));
    ASSERT_EQ(0x8000, read_reg(CS));
    ASSERT_EQ(0x2010, read_reg(IP));
}

TEST_F(EmulateFixture, CallIndirectInter)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    write_reg(SP, 0x0100);

    write_reg(BX, 0x0200);
    write_mem16(0x0202, 0x8000);
    write_mem16(0x0200, 0x2010);

    // call far [bx]
    set_instruction({0xff, 0x1f});

    emulate();

    ASSERT_EQ(0x00fc, read_reg(SP));
    ASSERT_EQ(0x2000, read_mem16(0x00fe, SS));
    ASSERT_EQ(0x0032, read_mem16(0x00fc, SS));
    EXPECT_EQ(0x8000, read_reg(CS));
    EXPECT_EQ(0x2010, read_reg(IP));
}
