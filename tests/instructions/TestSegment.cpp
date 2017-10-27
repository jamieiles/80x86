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

// 0x26: es:
// 0x2e: cs:
// 0x36: ss:
// 0x3e: ds:
//
// stack transfers don't get segment override used
// string destination doesn't get segment override used

struct MovOverrideTest {
    const char *segment;
    GPR segment_register;
    uint8_t prefix_byte;
};

class MovOverride : public EmulateFixture,
                    public ::testing::WithParamInterface<struct MovOverrideTest>
{
};
TEST_P(MovOverride, MemWriteSegmentOverriden)
{
    auto t = GetParam();

    SCOPED_TRACE(t.segment);

    write_mem16(0x100, mem_init_16, DS);
    write_reg(DS, 0x2000);
    write_mem16(0x100, mem_init_16, DS);

    write_reg(t.segment_register, 0x8000);
    // mov word [SEGMENT:0x0100], 0xaa55
    set_instruction({t.prefix_byte, 0xc7, 0x06, 0x00, 0x01, 0x55, 0xaa});

    emulate();

    ASSERT_EQ(0xaa55, read_mem16(0x0100, t.segment_register));
    EXPECT_EQ(0x2000, read_reg(DS));
    ASSERT_EQ(mem_init_16, read_mem16(0x0100, DS));
}

TEST_P(MovOverride, MemReadSegmentOverriden)
{
    auto t = GetParam();

    SCOPED_TRACE(t.segment);

    write_reg(t.segment_register, 0x8000);
    write_mem16(0x100, 0xa5a5, t.segment_register);
    // mov word ax, [SEGMENT:0x0100]
    set_instruction({t.prefix_byte, 0xa1, 0x00, 0x01});

    emulate();

    ASSERT_EQ(0xa5a5, read_reg(AX));
}
INSTANTIATE_TEST_CASE_P(SegmentOverride,
                        MovOverride,
                        ::testing::Values(MovOverrideTest{"ES", ES, 0x26},
                                          MovOverrideTest{"CS", CS, 0x2e},
                                          MovOverrideTest{"SS", SS, 0x36}));

TEST_F(EmulateFixture, DSSegmentOverrideIsNop)
{
    write_reg(DS, 0x8000);
    // mov word [ds:0x0100], 0xaa55
    set_instruction({0x3e, 0xc7, 0x06, 0x00, 0x01, 0x55, 0xaa});

    emulate();

    ASSERT_EQ(0xaa55, read_mem16(0x0100, DS));
}

TEST_F(EmulateFixture, PushMemSegmentOverride)
{
    write_reg(SS, 0x4000);
    write_reg(ES, 0x2000);
    write_reg(BX, 0x0100);
    write_reg(SP, 0x0080);

    write_mem16(0x0100, 0xaa55, ES);

    // push word [es:bx]
    set_instruction({0x26, 0xff, 0x37});

    emulate();

    ASSERT_EQ(read_reg(SP), 0x007e);
    ASSERT_EQ(read_mem16(0x007e, SS), 0xaa55);
}

TEST_F(EmulateFixture, PopMemSegmentOverride)
{
    write_reg(SS, 0x4000);
    write_reg(ES, 0x2000);
    write_reg(BX, 0x0100);
    write_reg(SP, 0x007e);

    write_mem16(0x007e, 0xaa55, SS);

    // pop word [es:bx]
    set_instruction({0x26, 0x8f, 0x07});

    emulate();

    ASSERT_EQ(read_reg(SP), 0x0080);
    ASSERT_EQ(read_mem16(0x0100, ES), 0xaa55);
}
