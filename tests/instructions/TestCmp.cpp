#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"
#include "Arithmetic.h"

static const std::vector<struct ArithmeticTest<uint8_t>> cmp8_tests = {
    {0, 0, 0, PF | ZF, false}, {0, 1, 0, PF | SF | CF | AF, false},
        {3, 2, 3, 0, false}, {0xff, 0xff, 0xff, PF | ZF, false},
        {0, 0xff, 0, CF | AF, false},
};

static const std::vector<struct ArithmeticTest<uint16_t>> cmp16_tests = {
    {0, 0, 0, PF | ZF, false}, {0, 1, 0, PF | SF | CF | AF, false},
        {3, 2, 3, 0, false}, {0xffff, 0xffff, 0xffff, PF | ZF, false},
        {0, 0xffff, 0, CF | AF, false},
};

INSTANTIATE_TEST_CASE_P(Cmp,
                        ArithmeticRegReg8Test,
                        ::testing::Values(
                            // cmp al, bl
                            Arith8Params({0x38, 0xd8}, cmp8_tests)));

INSTANTIATE_TEST_CASE_P(Cmp,
                        ArithmeticMemReg8Test,
                        ::testing::Values(
                            // cmp [bx], al
                            Arith8Params({0x38, 0x07}, cmp8_tests)));

INSTANTIATE_TEST_CASE_P(Cmp,
                        ArithmeticRegReg8TestReversedNoResult,
                        ::testing::Values(
                            // cmp bl, al
                            Arith8Params({0x3a, 0xd8}, cmp8_tests)));

INSTANTIATE_TEST_CASE_P(Cmp,
                        ArithmeticMemReg8TestReversedNoResult,
                        ::testing::Values(
                            // cmp al, [bx]
                            Arith8Params({0x3a, 0x07}, cmp8_tests)));

INSTANTIATE_TEST_CASE_P(Cmp,
                        ArithmeticRegReg16Test,
                        ::testing::Values(
                            // cmp ax, bx
                            Arith16Params({0x39, 0xd8}, cmp16_tests)));

INSTANTIATE_TEST_CASE_P(Cmp,
                        ArithmeticRegMem16Test,
                        ::testing::Values(
                            // adc [bx], ax
                            Arith16Params({0x39, 0x07}, cmp16_tests)));

INSTANTIATE_TEST_CASE_P(Cmp,
                        ArithmeticRegReg16TestReversedNoResult,
                        ::testing::Values(
                            // cmp bx, ax
                            Arith16Params({0x3b, 0xd8}, cmp16_tests)));

INSTANTIATE_TEST_CASE_P(Cmp,
                        ArithmeticMemReg16TestReversedNoResult,
                        ::testing::Values(
                            // cmp ax, bx
                            Arith16Params({0x3b, 0x07}, cmp16_tests)));

// Immediates

INSTANTIATE_TEST_CASE_P(
    Cmp,
    ArithmeticRegImmed8Test,
    ::testing::Values(
        // cmp bl, 1
        ArithImmed8Params(std::vector<uint8_t>{0x80, 0xfb, 0x01},
                          {2, 2, 0, false}),
        ArithImmed8Params(std::vector<uint8_t>{0x82, 0xfb, 0x01},
                          {2, 2, 0, false})));

INSTANTIATE_TEST_CASE_P(
    Cmp,
    ArithmeticMemImmed8Test,
    ::testing::Values(
        // cmp byte [bx], 1
        ArithImmed8Params(std::vector<uint8_t>{0x80, 0x3f, 0x01},
                          {2, 2, 0, false}),
        ArithImmed8Params(std::vector<uint8_t>{0x82, 0x3f, 0x01},
                          {2, 2, 0, false})));

INSTANTIATE_TEST_CASE_P(Cmp,
                        ArithmeticRegImmed16Test,
                        ::testing::Values(
                            // cmp bx, 1
                            ArithImmed16Params(std::vector<uint8_t>{0x81, 0xfb,
                                                                    0x01, 0x0},
                                               {2, 2, 0, false})));

INSTANTIATE_TEST_CASE_P(Cmp,
                        ArithmeticMemImmed16Test,
                        ::testing::Values(
                            // cmp word [bx], 1
                            ArithImmed16Params(std::vector<uint8_t>{0x81, 0x3f,
                                                                    0x01, 0x00},
                                               {2, 2, 0, false})));

INSTANTIATE_TEST_CASE_P(
    Cmp,
    ArithmeticRegImmed16TestExtend,
    ::testing::Values(
        // cmp bx, -1
        ArithImmed16Params(std::vector<uint8_t>{0x83, 0xfb, 0xff},
                           {1, 1, CF | AF, false}),
        // cmp bx, 1
        ArithImmed16Params(std::vector<uint8_t>{0x83, 0xfb, 0x01},
                           {2, 2, 0, false})));

INSTANTIATE_TEST_CASE_P(
    Cmp,
    ArithmeticMemImmed16TestExtend,
    ::testing::Values(
        // cmp word [bx], -1
        ArithImmed16Params(std::vector<uint8_t>{0x83, 0x3f, 0xff},
                           {1, 1, CF | AF, false}),
        // cmp word [bx], 1
        ArithImmed16Params(std::vector<uint8_t>{0x83, 0x3f, 0x01},
                           {2, 2, 0, false})));

INSTANTIATE_TEST_CASE_P(Cmp,
                        ArithmeticAlImmedTest,
                        ::testing::Values(
                            // cmp al, 1
                            ArithImmed8Params(std::vector<uint8_t>{0x3c, 0x01},
                                              {2, 2, 0, false})));

INSTANTIATE_TEST_CASE_P(Cmp,
                        ArithmeticAxImmedTest,
                        ::testing::Values(
                            // cmp ax, 1
                            ArithImmed16Params(std::vector<uint8_t>{0x3d, 0x01,
                                                                    0x0},
                                               {2, 2, 0, false})));
