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

#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

class InvalidOpcodeFixture
    : public EmulateFixture,
      public ::testing::WithParamInterface<std::vector<uint8_t>>
{
};
TEST_P(InvalidOpcodeFixture, InvalidOpcode)
{
    write_mem16(VEC_INVALID_OPCODE + 2, 0x8000, CS); // CS
    write_mem16(VEC_INVALID_OPCODE + 0, 0x0100, CS); // IP

    write_reg(SP, 0x100);
    write_reg(CS, 0x7c00);
    write_reg(IP, 0x0001);
    write_flags(IF);

    set_instruction(GetParam());

    emulate();

    EXPECT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), FLAGS_STUCK_BITS);
    EXPECT_EQ(read_reg(CS), 0x8000);
    EXPECT_EQ(read_reg(IP), 0x0100);
    EXPECT_EQ(read_reg(SP), 0x0100 - 6);

    EXPECT_EQ(read_mem16(0x100 - 2, SS), FLAGS_STUCK_BITS | IF);
    EXPECT_EQ(read_mem16(0x100 - 4, SS), 0x7c00);
    // Return to the following instruction
    EXPECT_EQ(read_mem16(0x100 - 6, SS), 0x0001 + GetParam().size());
}
INSTANTIATE_TEST_CASE_P(InvalidOpcode,
                        InvalidOpcodeFixture,
                        ::testing::Values(std::vector<uint8_t>{0x0f},
                                          std::vector<uint8_t>{0x63},
                                          std::vector<uint8_t>{0x64},
                                          std::vector<uint8_t>{0x65},
                                          std::vector<uint8_t>{0x66},
                                          std::vector<uint8_t>{0x67},
                                          std::vector<uint8_t>{0xf1},
                                          std::vector<uint8_t>{0xfe, 0xff},
                                          std::vector<uint8_t>{0xff, 0xff}));

class UnimplementedOpcodeFixture
    : public EmulateFixture,
      public ::testing::WithParamInterface<std::vector<uint8_t>>
{
};
TEST_P(UnimplementedOpcodeFixture, UnimplementedOpcode)
{
    set_instruction(GetParam());

    emulate();

    ASSERT_FALSE(instruction_had_side_effects());
}
INSTANTIATE_TEST_CASE_P(UnimplementedOpcode,
                        UnimplementedOpcodeFixture,
                        ::testing::Values(
                            // inc
                            std::vector<uint8_t>{0xfe, 2 << 3},
                            std::vector<uint8_t>{0xfe, 3 << 3},
                            std::vector<uint8_t>{0xfe, 4 << 3},
                            std::vector<uint8_t>{0xfe, 5 << 3},
                            std::vector<uint8_t>{0xfe, 6 << 3},
                            // pop
                            std::vector<uint8_t>{0x8f, 1 << 3},
                            std::vector<uint8_t>{0x8f, 2 << 3},
                            std::vector<uint8_t>{0x8f, 3 << 3},
                            std::vector<uint8_t>{0x8f, 4 << 3},
                            std::vector<uint8_t>{0x8f, 5 << 3},
                            std::vector<uint8_t>{0x8f, 6 << 3},
                            std::vector<uint8_t>{0x8f, 7 << 3},
                            // mov
                            std::vector<uint8_t>{0xc6, 1 << 3},
                            std::vector<uint8_t>{0xc6, 2 << 3},
                            std::vector<uint8_t>{0xc6, 3 << 3},
                            std::vector<uint8_t>{0xc6, 4 << 3},
                            std::vector<uint8_t>{0xc6, 5 << 3},
                            std::vector<uint8_t>{0xc6, 6 << 3},
                            std::vector<uint8_t>{0xc6, 7 << 3},
                            std::vector<uint8_t>{0xc7, 1 << 3},
                            std::vector<uint8_t>{0xc7, 2 << 3},
                            std::vector<uint8_t>{0xc7, 3 << 3},
                            std::vector<uint8_t>{0xc7, 4 << 3},
                            std::vector<uint8_t>{0xc7, 5 << 3},
                            std::vector<uint8_t>{0xc7, 6 << 3},
                            std::vector<uint8_t>{0xc7, 7 << 3},
                            // lds
                            std::vector<uint8_t>{0xc5, 0xc0},
                            // les
                            std::vector<uint8_t>{0xc4, 0xc0},
                            // lea
                            std::vector<uint8_t>{0x8d, 0xc0},
                            // call indirect inter reg
                            std::vector<uint8_t>{0xff, 3 << 6 | 3 << 3}));

TEST_F(EmulateFixture, TooLongTraps)
{
    write_mem16(VEC_INVALID_OPCODE + 2, 0x8000, CS); // CS
    write_mem16(VEC_INVALID_OPCODE + 0, 0x0100, CS); // IP

    write_reg(SP, 0x100);
    write_reg(CS, 0x7c00);
    write_reg(IP, 0x0001);
    write_flags(IF);

    std::vector<uint8_t> instr;
    for (auto i = 0; i < 15; ++i)
        instr.push_back(0xf0);
    instr.push_back(0x90);
    set_instruction(instr);

    emulate(1, false);

    EXPECT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), FLAGS_STUCK_BITS);
    EXPECT_EQ(read_reg(CS), 0x8000);
    EXPECT_EQ(read_reg(IP), 0x0100);
    EXPECT_EQ(read_reg(SP), 0x0100 - 6);

    EXPECT_EQ(read_mem16(0x100 - 2, SS), FLAGS_STUCK_BITS | IF);
    EXPECT_EQ(read_mem16(0x100 - 4, SS), 0x7c00);
    // Return to the following instruction (length % 16).
    EXPECT_EQ(read_mem16(0x100 - 6, SS), 0x0001);
}
