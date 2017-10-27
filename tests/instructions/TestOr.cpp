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
#include "Arithmetic.h"

static const std::vector<struct ArithmeticTest<uint8_t>> or8_tests = {
    {0, 0, 0, PF | ZF, false}, {0xf, 0xf, 0x0f, PF, false},
        {0x0f, 0xf0, 0xff, PF | SF, false}, {0x0f, 0x01, 0x0f, PF, false},
};

static const std::vector<struct ArithmeticTest<uint16_t>> or16_tests = {
    {0, 0, 0, PF | ZF, false}, {0xf, 0xf, 0x0f, PF, false},
        {0x00ff, 0xff00, 0xffff, PF | SF, false},
        {0x00ff, 0x0f01, 0x0fff, PF, false},
};

INSTANTIATE_TEST_CASE_P(Or,
                        ArithmeticRegReg8Test,
                        ::testing::Values(
                            // or al, bl
                            Arith8Params({0x08, 0xd8}, or8_tests)));

INSTANTIATE_TEST_CASE_P(Or,
                        ArithmeticMemReg8Test,
                        ::testing::Values(
                            // or [bx], al
                            Arith8Params({0x08, 0x07}, or8_tests)));

INSTANTIATE_TEST_CASE_P(Or,
                        ArithmeticRegReg8TestReversed,
                        ::testing::Values(
                            // or bl, al
                            Arith8Params({0x0a, 0xd8}, or8_tests)));

INSTANTIATE_TEST_CASE_P(Or,
                        ArithmeticMemReg8TestReversed,
                        ::testing::Values(
                            // or al, [bx]
                            Arith8Params({0x0a, 0x07}, or8_tests)));

INSTANTIATE_TEST_CASE_P(Or,
                        ArithmeticRegReg16Test,
                        ::testing::Values(
                            // or ax, bx
                            Arith16Params({0x09, 0xd8}, or16_tests)));

INSTANTIATE_TEST_CASE_P(Or,
                        ArithmeticRegMem16Test,
                        ::testing::Values(
                            // adc [bx], ax
                            Arith16Params({0x09, 0x07}, or16_tests)));

INSTANTIATE_TEST_CASE_P(Or,
                        ArithmeticRegReg16TestReversed,
                        ::testing::Values(
                            // or bx, ax
                            Arith16Params({0x0b, 0xd8}, or16_tests)));

INSTANTIATE_TEST_CASE_P(Or,
                        ArithmeticMemReg16TestReversed,
                        ::testing::Values(
                            // or ax, bx
                            Arith16Params({0x0b, 0x07}, or16_tests)));

INSTANTIATE_TEST_CASE_P(Or,
                        ArithmeticRegImmed8Test,
                        ::testing::Values(
                            // or bl, 1
                            ArithImmed8Params(std::vector<uint8_t>{0x80, 0xcb,
                                                                   0x01},
                                              {2, 3, PF, false})));

INSTANTIATE_TEST_CASE_P(Or,
                        ArithmeticMemImmed8Test,
                        ::testing::Values(
                            // or byte [bx], 1
                            ArithImmed8Params(std::vector<uint8_t>{0x80, 0x0f,
                                                                   0x01},
                                              {2, 3, PF, false})));

INSTANTIATE_TEST_CASE_P(Or,
                        ArithmeticRegImmed16Test,
                        ::testing::Values(
                            // or bx, 1
                            ArithImmed16Params(std::vector<uint8_t>{0x81, 0xcb,
                                                                    0x01, 0x0},
                                               {2, 3, PF, false})));

INSTANTIATE_TEST_CASE_P(Or,
                        ArithmeticMemImmed16Test,
                        ::testing::Values(
                            // or word [bx], 1
                            ArithImmed16Params(std::vector<uint8_t>{0x81, 0x0f,
                                                                    0x01, 0x00},
                                               {2, 3, PF, false})));

INSTANTIATE_TEST_CASE_P(
    Or,
    ArithmeticRegImmed16TestExtend,
    ::testing::Values(
        // or bx, 1
        ArithImmed16Params(std::vector<uint8_t>{0x83, 0xcb, 0x01},
                           {0x0100, 0x0101, 0, false}),
        // or bx, -1
        ArithImmed16Params(std::vector<uint8_t>{0x83, 0xcb, 0xff},
                           {0x0101, 0xffff, PF | SF, false})));

INSTANTIATE_TEST_CASE_P(
    Or,
    ArithmeticMemImmed16TestExtend,
    ::testing::Values(
        // or [bx], 1
        ArithImmed16Params(std::vector<uint8_t>{0x83, 0x0f, 0x01},
                           {0x0100, 0x0101, 0, false}),
        // or [bx], -1
        ArithImmed16Params(std::vector<uint8_t>{0x83, 0x0f, 0xff},
                           {0x0101, 0xffff, PF | SF, false})));

INSTANTIATE_TEST_CASE_P(Or,
                        ArithmeticAlImmedTest,
                        ::testing::Values(
                            // or al, 1
                            ArithImmed8Params(std::vector<uint8_t>{0x0c, 0x01},
                                              {2, 3, PF, false})));

INSTANTIATE_TEST_CASE_P(Or,
                        ArithmeticAxImmedTest,
                        ::testing::Values(
                            // or ax, 1
                            ArithImmed16Params(std::vector<uint8_t>{0x0d, 0x01,
                                                                    0x0},
                                               {2, 3, PF, false})));
