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

static const std::vector<struct ShiftTest<uint8_t>> rcr8_shift1_tests = {
    {1, 0, 0, 0, 0}, {1, 0x80, 0x40, 0, OF}, {1, 0, 0x80, CF, OF},
        {1, 2, 1, 0, 0}, {1, 0x80, 0xc0, CF, 0},
};

static const std::vector<struct ShiftTest<uint16_t>> rcr16_shift1_tests = {
    {1, 0, 0, 0, 0}, {1, 0x8000, 0x4000, 0, OF}, {1, 0, 0x8000, CF, OF},
        {1, 2, 1, 0, 0}, {1, 0x8000, 0xc000, CF, 0},
};

INSTANTIATE_TEST_CASE_P(Rcrg1,
                        ShiftReg8Test,
                        ::testing::Values(
                            // rcr al, 1
                            Shift8Params({0xd0, 0xd8}, rcr8_shift1_tests)));

INSTANTIATE_TEST_CASE_P(Rcrg1,
                        ShiftMem8Test,
                        ::testing::Values(
                            // rcr byte [bx], 1
                            Shift8Params({0xd0, 0x1f}, rcr8_shift1_tests)));

INSTANTIATE_TEST_CASE_P(Rcrg1,
                        ShiftReg16Test,
                        ::testing::Values(
                            // rcr ax, 1
                            Shift16Params({0xd1, 0xd8}, rcr16_shift1_tests)));

INSTANTIATE_TEST_CASE_P(Rcrg1,
                        ShiftMem16Test,
                        ::testing::Values(
                            // rcr word [bx], 1
                            Shift16Params({0xd1, 0x1f}, rcr16_shift1_tests)));

static const std::vector<struct ShiftTest<uint8_t>> rcr8_shiftN_tests = {
    {0, 1, 1, 0, 0}, {1, 0, 0, 0, 0}, {1, 0x80, 0x40, 0, 0},
        {1, 0, 0x80, CF, 0}, {1, 2, 1, 0, 0}, {1, 0x80, 0xc0, CF, 0},
        {8, 0, 0, 0, 0}, {7, 0x80, 0x1, 0, 0}, {8, 0x80, 0x00, 0, CF},
};

static const std::vector<struct ShiftTest<uint16_t>> rcr16_shiftN_tests = {
    {0, 1, 1, 0, 0}, {1, 0, 0, 0, 0}, {1, 0x8000, 0x4000, 0, 0},
        {1, 0, 0x8000, CF, 0}, {1, 2, 1, 0, 0}, {1, 0x8000, 0xc000, CF, 0},
        {16, 0, 0, 0, 0}, {15, 0x8000, 0x1, 0, 0}, {16, 0x8000, 0x00, 0, CF},
};

INSTANTIATE_TEST_CASE_P(RcrgN,
                        ShiftReg8Test,
                        ::testing::Values(
                            // rcr al, N
                            Shift8Params({0xd2, 0xd8}, rcr8_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(RcrgN,
                        ShiftMem8Test,
                        ::testing::Values(
                            // rcr byte [bx], N
                            Shift8Params({0xd2, 0x1f}, rcr8_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(RcrgN,
                        ShiftReg16Test,
                        ::testing::Values(
                            // rcr ax, N
                            Shift16Params({0xd3, 0xd8}, rcr16_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(RcrgN,
                        ShiftMem16Test,
                        ::testing::Values(
                            // rcr word [bx], N
                            Shift16Params({0xd3, 0x1f}, rcr16_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(RcrN,
                        ShiftRegImm8Test,
                        ::testing::Values(
                            // rcr ax, imm8
                            Shift8Params({0xc0, 0xd8}, rcr8_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(RcrN,
                        ShiftMemImm8Test,
                        ::testing::Values(
                            // rcr word [bx], imm8
                            Shift8Params({0xc0, 0x1f}, rcr8_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(RcrN,
                        ShiftRegImm16Test,
                        ::testing::Values(
                            // rcr ax, imm16
                            Shift16Params({0xc1, 0xd8}, rcr16_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(RcrN,
                        ShiftMemImm16Test,
                        ::testing::Values(
                            // rcr word [bx], imm16
                            Shift16Params({0xc1, 0x1f}, rcr16_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(
    RcrgCL,
    ShiftCLTest,
    ::testing::Values(ShiftCLTestParams{4, 0x40, {0xd2, 0xd1}}));
