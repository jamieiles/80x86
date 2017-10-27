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

TEST_F(EmulateFixture, XchgRegReg8)
{
    // xchg al, bl
    set_instruction({0x86, 0xc3});
    write_reg(AL, 0xaa);
    write_reg(BL, 0xbb);

    emulate();

    ASSERT_EQ(read_reg(AL), 0xbb);
    ASSERT_EQ(read_reg(BL), 0xaa);
}

TEST_F(EmulateFixture, XchgRegReg16)
{
    // xchg bx, cx
    set_instruction({0x87, 0xd9});
    write_reg(BX, 0xbbbb);
    write_reg(CX, 0xcccc);

    emulate();

    ASSERT_EQ(read_reg(BX), 0xcccc);
    ASSERT_EQ(read_reg(CX), 0xbbbb);
}

TEST_F(EmulateFixture, XchgRegMem8)
{
    // xchg al, [0x1234]
    set_instruction({0x86, 0x06, 0x34, 0x12});
    write_mem8(0x1234, 0x12);
    write_reg(AL, 0xaa);

    emulate();

    ASSERT_EQ(read_reg(AL), 0x12);
    ASSERT_EQ(read_mem8(0x1234), 0xaa);
}

TEST_F(EmulateFixture, XchgRegMem16)
{
    // xchg ax, [0x1234]
    set_instruction({0x87, 0x06, 0x34, 0x12});
    write_mem16(0x1234, 0x1234);
    write_reg(AX, 0xaaaa);

    emulate();

    ASSERT_EQ(read_reg(AX), 0x1234);
    ASSERT_EQ(read_mem16(0x1234), 0xaaaa);
}

TEST_F(EmulateFixture, XchgALALNop)
{
    // xchg al, al
    set_instruction({0x86, 0xc0});
    write_reg(AL, 0xaa);

    emulate();

    ASSERT_EQ(read_reg(AL), 0xaa);
}

class XchgAXFixture
    : public EmulateFixture,
      public ::testing::WithParamInterface<std::pair<uint8_t, GPR>>
{
};
TEST_P(XchgAXFixture, XchgRegAccumulator)
{
    // xchg ax, ?x
    set_instruction({GetParam().first});
    write_reg(AX, 0xaaaa);
    write_reg(GetParam().second, 0xbbbb);

    emulate();

    ASSERT_EQ(read_reg(AX), 0xbbbb);
    ASSERT_EQ(read_reg(GetParam().second), 0xaaaa);
}
INSTANTIATE_TEST_CASE_P(
    XchgAccumulator,
    XchgAXFixture,
    ::testing::Values(std::make_pair<uint8_t, GPR>(0x91, CX),
                      std::make_pair<uint8_t, GPR>(0x92, DX),
                      std::make_pair<uint8_t, GPR>(0x93, BX),
                      std::make_pair<uint8_t, GPR>(0x94, SP),
                      std::make_pair<uint8_t, GPR>(0x95, BP),
                      std::make_pair<uint8_t, GPR>(0x96, SI),
                      std::make_pair<uint8_t, GPR>(0x97, DI)));
