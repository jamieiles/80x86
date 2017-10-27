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

TEST_F(EmulateFixture, ClcClearsCarry)
{
    write_flags(CF);

    set_instruction({0xf8});

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), FLAGS_STUCK_BITS);
}

TEST_F(EmulateFixture, ClcDoesntSetCarry)
{
    write_flags(0);

    set_instruction({0xf8});

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), FLAGS_STUCK_BITS);
}

TEST_F(EmulateFixture, CmcInverts0)
{
    write_flags(0);

    set_instruction({0xf5});

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), FLAGS_STUCK_BITS | CF);
}

TEST_F(EmulateFixture, CmcInverts1)
{
    write_flags(CF);

    set_instruction({0xf5});

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), FLAGS_STUCK_BITS);
}

TEST_F(EmulateFixture, StcSetsCarry)
{
    write_flags(0);

    set_instruction({0xf9});

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), FLAGS_STUCK_BITS | CF);
}

TEST_F(EmulateFixture, StcDoesntClearCarry)
{
    write_flags(CF);

    set_instruction({0xf9});

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), FLAGS_STUCK_BITS | CF);
}

TEST_F(EmulateFixture, CldClearsDirection)
{
    write_flags(DF);

    set_instruction({0xfc});

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), FLAGS_STUCK_BITS);
}

TEST_F(EmulateFixture, CldDoesntSetDirection)
{
    write_flags(0);

    set_instruction({0xfc});

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), FLAGS_STUCK_BITS);
}

TEST_F(EmulateFixture, StdSetsDirection)
{
    write_flags(0);

    set_instruction({0xfd});

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), FLAGS_STUCK_BITS | DF);
}

TEST_F(EmulateFixture, StdDoesntClearDirection)
{
    write_flags(DF);

    set_instruction({0xfd});

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), FLAGS_STUCK_BITS | DF);
}

TEST_F(EmulateFixture, CliClearsInterrupt)
{
    write_flags(IF);

    set_instruction({0xfa});

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), FLAGS_STUCK_BITS);
}

TEST_F(EmulateFixture, CliDoesntSetInterrupt)
{
    write_flags(0);

    set_instruction({0xfa});

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), FLAGS_STUCK_BITS);
}

TEST_F(EmulateFixture, StiSetsInterrupt)
{
    write_flags(0);

    set_instruction({0xfb});

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), FLAGS_STUCK_BITS | IF);
}

TEST_F(EmulateFixture, StiDoesntClearInterrupt)
{
    write_flags(IF);

    set_instruction({0xfb});

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), FLAGS_STUCK_BITS | IF);
}
