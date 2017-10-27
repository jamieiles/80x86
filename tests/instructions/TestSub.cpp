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

static const std::vector<struct ArithmeticTest<uint8_t>> sub8_tests = {
    {0, 0, 0, PF | ZF, false}, {0, 1, 0xff, PF | SF | CF | AF, false},
        {3, 2, 1, 0, false}, {0xff, 0xff, 0, PF | ZF, false},
        {0, 0xff, 1, CF | AF, false},
};

static const std::vector<struct ArithmeticTest<uint16_t>> sub16_tests = {
    {0, 0, 0, PF | ZF, false}, {0, 1, 0xffff, PF | SF | CF | AF, false},
        {3, 2, 1, 0, false}, {0xffff, 0xffff, 0, PF | ZF, false},
        {0, 0xffff, 1, CF | AF, false},
};

INSTANTIATE_TEST_CASE_P(Sub,
                        ArithmeticRegReg8Test,
                        ::testing::Values(
                            // sub al, bl
                            Arith8Params({0x28, 0xd8}, sub8_tests)));

INSTANTIATE_TEST_CASE_P(Sub,
                        ArithmeticMemReg8Test,
                        ::testing::Values(
                            // sub [bx], al
                            Arith8Params({0x28, 0x07}, sub8_tests)));

INSTANTIATE_TEST_CASE_P(Sub,
                        ArithmeticRegReg8TestReversed,
                        ::testing::Values(
                            // sub bl, al
                            Arith8Params({0x2a, 0xd8}, sub8_tests)));

INSTANTIATE_TEST_CASE_P(Sub,
                        ArithmeticMemReg8TestReversed,
                        ::testing::Values(
                            // sub al, [bx]
                            Arith8Params({0x2a, 0x07}, sub8_tests)));

INSTANTIATE_TEST_CASE_P(Sub,
                        ArithmeticRegReg16Test,
                        ::testing::Values(
                            // sub ax, bx
                            Arith16Params({0x29, 0xd8}, sub16_tests)));

INSTANTIATE_TEST_CASE_P(Sub,
                        ArithmeticRegMem16Test,
                        ::testing::Values(
                            // adc [bx], ax
                            Arith16Params({0x29, 0x07}, sub16_tests)));

INSTANTIATE_TEST_CASE_P(Sub,
                        ArithmeticRegReg16TestReversed,
                        ::testing::Values(
                            // sub bx, ax
                            Arith16Params({0x2b, 0xd8}, sub16_tests)));

INSTANTIATE_TEST_CASE_P(Sub,
                        ArithmeticMemReg16TestReversed,
                        ::testing::Values(
                            // sub ax, bx
                            Arith16Params({0x2b, 0x07}, sub16_tests)));

// Immediates

INSTANTIATE_TEST_CASE_P(
    Sub,
    ArithmeticRegImmed8Test,
    ::testing::Values(
        // sub bl, 1
        ArithImmed8Params(std::vector<uint8_t>{0x80, 0xeb, 0x01},
                          {2, 1, 0, false}),
        ArithImmed8Params(std::vector<uint8_t>{0x82, 0xeb, 0x01},
                          {2, 1, 0, false})));

INSTANTIATE_TEST_CASE_P(
    Sub,
    ArithmeticMemImmed8Test,
    ::testing::Values(
        // sub byte [bx], 1
        ArithImmed8Params(std::vector<uint8_t>{0x80, 0x2f, 0x01},
                          {2, 1, 0, false}),
        ArithImmed8Params(std::vector<uint8_t>{0x82, 0x2f, 0x01},
                          {2, 1, 0, false})));

INSTANTIATE_TEST_CASE_P(Sub,
                        ArithmeticRegImmed16Test,
                        ::testing::Values(
                            // sub bx, 1
                            ArithImmed16Params(std::vector<uint8_t>{0x81, 0xeb,
                                                                    0x01, 0x0},
                                               {2, 1, 0, false})));

INSTANTIATE_TEST_CASE_P(Sub,
                        ArithmeticMemImmed16Test,
                        ::testing::Values(
                            // sub word [bx], 1
                            ArithImmed16Params(std::vector<uint8_t>{0x81, 0x2f,
                                                                    0x01, 0x00},
                                               {2, 1, 0, false})));

INSTANTIATE_TEST_CASE_P(
    Sub,
    ArithmeticRegImmed16TestExtend,
    ::testing::Values(
        // sub bx, -1
        ArithImmed16Params(std::vector<uint8_t>{0x83, 0xeb, 0xff},
                           {1, 2, CF | AF, false}),
        // sub bx, 1
        ArithImmed16Params(std::vector<uint8_t>{0x83, 0xeb, 0x01},
                           {2, 1, 0, false})));

INSTANTIATE_TEST_CASE_P(
    Sub,
    ArithmeticMemImmed16TestExtend,
    ::testing::Values(
        // sub word [bx], -1
        ArithImmed16Params(std::vector<uint8_t>{0x83, 0x2f, 0xff},
                           {1, 2, CF | AF, false}),
        // sub word [bx], 1
        ArithImmed16Params(std::vector<uint8_t>{0x83, 0x2f, 0x01},
                           {2, 1, 0, false})));

INSTANTIATE_TEST_CASE_P(Sub,
                        ArithmeticAlImmedTest,
                        ::testing::Values(
                            // sub al, 1
                            ArithImmed8Params(std::vector<uint8_t>{0x2c, 0x01},
                                              {2, 1, 0, false})));

INSTANTIATE_TEST_CASE_P(Sub,
                        ArithmeticAxImmedTest,
                        ::testing::Values(
                            // sub ax, 1
                            ArithImmed16Params(std::vector<uint8_t>{0x2d, 0x01,
                                                                    0x0},
                                               {2, 1, 0, false})));
