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

#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"
#include "Shift.h"

static const std::vector<struct ShiftTest<uint8_t>> ror8_shift1_tests = {
    {1, 0, 0, 0, 0}, {1, 2, 1, 0, 0}, {1, 0x81, 0xc0, 0, CF},
        {1, 0x01, 0x80, 0, CF | OF},
};

static const std::vector<struct ShiftTest<uint16_t>> ror16_shift1_tests = {
    {1, 0, 0, 0, 0}, {1, 2, 1, 0, 0}, {1, 0x8001, 0xc000, 0, CF},
        {1, 0x0001, 0x8000, 0, CF | OF},
};

INSTANTIATE_TEST_CASE_P(Ror1,
                        ShiftReg8Test,
                        ::testing::Values(
                            // ror al, 1
                            Shift8Params({0xd0, 0xc8}, ror8_shift1_tests)));

INSTANTIATE_TEST_CASE_P(Ror1,
                        ShiftMem8Test,
                        ::testing::Values(
                            // ror byte [bx], 1
                            Shift8Params({0xd0, 0x0f}, ror8_shift1_tests)));

INSTANTIATE_TEST_CASE_P(Ror1,
                        ShiftReg16Test,
                        ::testing::Values(
                            // ror ax, 1
                            Shift16Params({0xd1, 0xc8}, ror16_shift1_tests)));

INSTANTIATE_TEST_CASE_P(Ror1,
                        ShiftMem16Test,
                        ::testing::Values(
                            // ror word [bx], 1
                            Shift16Params({0xd1, 0x0f}, ror16_shift1_tests)));

static const std::vector<struct ShiftTest<uint8_t>> ror8_shiftN_tests = {
    {0, 1, 1, 0, 0}, {1, 0, 0, 0, 0}, {1, 2, 1, 0, 0}, {1, 0x81, 0xc0, 0, CF},
        {1, 0x01, 0x80, 0, CF},

        {8, 0, 0, 0, 0}, {7, 0x80, 1, 0, 0}, {8, 1, 0x01, 0, 0},
};

static const std::vector<struct ShiftTest<uint16_t>> ror16_shiftN_tests = {
    {0, 1, 1, 0, 0}, {1, 0, 0, 0, 0}, {1, 2, 1, 0, 0},
        {1, 0x8001, 0xc000, 0, CF}, {1, 0x0001, 0x8000, 0, CF},

        {16, 0, 0, 0, 0}, {15, 0x8000, 1, 0, 0}, {16, 1, 0x01, 0, 0},
};

INSTANTIATE_TEST_CASE_P(RorN,
                        ShiftReg8Test,
                        ::testing::Values(
                            // ror al, N
                            Shift8Params({0xd2, 0xc8}, ror8_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(RorN,
                        ShiftMem8Test,
                        ::testing::Values(
                            // ror byte [bx], N
                            Shift8Params({0xd2, 0x0f}, ror8_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(RorN,
                        ShiftReg16Test,
                        ::testing::Values(
                            // ror ax, N
                            Shift16Params({0xd3, 0xc8}, ror16_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(RorN,
                        ShiftMem16Test,
                        ::testing::Values(
                            // ror word [bx], N
                            Shift16Params({0xd3, 0x0f}, ror16_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(RorN,
                        ShiftRegImm8Test,
                        ::testing::Values(
                            // ror ax, imm8
                            Shift8Params({0xc0, 0xc8}, ror8_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(RorN,
                        ShiftMemImm8Test,
                        ::testing::Values(
                            // ror word [bx], imm8
                            Shift8Params({0xc0, 0x0f}, ror8_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(RorN,
                        ShiftRegImm16Test,
                        ::testing::Values(
                            // ror ax, imm16
                            Shift16Params({0xc1, 0xc8}, ror16_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(RorN,
                        ShiftMemImm16Test,
                        ::testing::Values(
                            // ror word [bx], imm16
                            Shift16Params({0xc1, 0x0f}, ror16_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(
    RorCL,
    ShiftCLTest,
    ::testing::Values(ShiftCLTestParams{2, 0x80, {0xd2, 0xc9}}));
