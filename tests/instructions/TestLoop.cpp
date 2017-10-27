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

TEST_F(EmulateFixture, LoopNotTaken)
{
    write_reg(CX, 1);
    write_reg(IP, 0x100);

    // loop ip-3
    set_instruction({0xe2, 0xfd});

    emulate();

    ASSERT_EQ(0x102, read_reg(IP));
}

TEST_F(EmulateFixture, LoopTaken)
{
    write_reg(CX, 2);
    write_reg(IP, 0x100);

    // loop ip-3
    set_instruction({0xe2, 0xfd});

    emulate();

    ASSERT_EQ(0x0ff, read_reg(IP));
}

TEST_F(EmulateFixture, LoopeNotTakenNonZero)
{
    write_reg(CX, 2);
    write_reg(IP, 0x100);
    write_flags(0);

    // loope ip-3
    set_instruction({0xe1, 0xfd});

    emulate();

    ASSERT_EQ(0x102, read_reg(IP));
}

TEST_F(EmulateFixture, LoopeNotTaken)
{
    write_reg(CX, 1);
    write_reg(IP, 0x100);
    write_flags(ZF);

    // loope ip-3
    set_instruction({0xe1, 0xfd});

    emulate();

    ASSERT_EQ(0x102, read_reg(IP));
}

TEST_F(EmulateFixture, LoopeTaken)
{
    write_reg(CX, 2);
    write_reg(IP, 0x100);
    write_flags(ZF);

    // loope ip-3
    set_instruction({0xe1, 0xfd});

    emulate();

    ASSERT_EQ(0x0ff, read_reg(IP));
}

TEST_F(EmulateFixture, LoopnzNotTakenNonZero)
{
    write_reg(CX, 1);
    write_reg(IP, 0x100);
    write_flags(0);

    // loopnz ip-3
    set_instruction({0xe0, 0xfd});

    emulate();

    ASSERT_EQ(0x102, read_reg(IP));
}

TEST_F(EmulateFixture, LoopnzNotTaken)
{
    write_reg(CX, 2);
    write_reg(IP, 0x100);
    write_flags(ZF);

    // loopnz ip-3
    set_instruction({0xe0, 0xfd});

    emulate();

    ASSERT_EQ(0x102, read_reg(IP));
}

TEST_F(EmulateFixture, LoopnzTaken)
{
    write_reg(CX, 2);
    write_reg(IP, 0x100);
    write_flags(0);

    // loopnz ip-3
    set_instruction({0xe0, 0xfd});

    emulate();

    ASSERT_EQ(0x0ff, read_reg(IP));
}
