#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"
#include "Arithmetic.h"

static const std::vector<struct ArithmeticTest<uint8_t>> test8_tests = {
    {0, 0, 0, PF | ZF, false}, {0xf, 0xf, 0x0f, PF, false},
        {0x0f, 0xf0, 0x0f, PF | ZF, false}, {0x0f, 0x01, 0x0f, 0, false},
};

static const std::vector<struct ArithmeticTest<uint16_t>> test16_tests = {
    {0, 0, 0, PF | ZF, false}, {0xf, 0xf, 0x0f, PF, false},
        {0x000f, 0xf000, 0xf, PF | ZF, false},
        {0xf00f, 0xf000, 0xf00f, PF | SF, false}, {0x0f, 0x01, 0x0f, 0, false},
};

INSTANTIATE_TEST_CASE_P(Test,
                        ArithmeticRegReg8Test,
                        ::testing::Values(
                            // test al, bl
                            Arith8Params({0x84, 0xd8}, test8_tests)));

INSTANTIATE_TEST_CASE_P(Test,
                        ArithmeticMemReg8Test,
                        ::testing::Values(
                            // test [bx], al
                            Arith8Params({0x84, 0x07}, test8_tests)));

INSTANTIATE_TEST_CASE_P(Test,
                        ArithmeticRegReg16Test,
                        ::testing::Values(
                            // test ax, bx
                            Arith16Params({0x85, 0xd8}, test16_tests)));

INSTANTIATE_TEST_CASE_P(Test,
                        ArithmeticRegMem16Test,
                        ::testing::Values(
                            // adc [bx], ax
                            Arith16Params({0x85, 0x07}, test16_tests)));

INSTANTIATE_TEST_CASE_P(Test,
                        ArithmeticRegImmed8Test,
                        ::testing::Values(
                            // test bl, 1
                            ArithImmed8Params(std::vector<uint8_t>{0xf6, 0xc3,
                                                                   0x01},
                                              {1, 1, 0, false})));

INSTANTIATE_TEST_CASE_P(
    Test,
    ArithmeticMemImmed8Test,
    ::testing::Values(
        // test byte [bx], 1
        ArithImmed8Params(std::vector<uint8_t>{0xf6, 0x07, 0x01},
                          {1, 1, 0, false}),
        // test byte [bx], 1
        ArithImmed8Params(std::vector<uint8_t>{0xf6, 0x0f, 0x01},
                          {1, 1, 0, false})));

INSTANTIATE_TEST_CASE_P(
    Test,
    ArithmeticRegImmed16Test,
    ::testing::Values(
        // test bx, 1
        ArithImmed16Params(std::vector<uint8_t>{0xf7, 0xc3, 0x01, 0x0},
                           {1, 1, 0, false}),
        // test bx, 1
        ArithImmed16Params(std::vector<uint8_t>{0xf7, 0xcb, 0x01, 0x0},
                           {1, 1, 0, false})));

INSTANTIATE_TEST_CASE_P(
    Test,
    ArithmeticMemImmed16Test,
    ::testing::Values(
        // test word [bx], 1
        ArithImmed16Params(std::vector<uint8_t>{0xf7, 0x07, 0x01, 0x00},
                           {1, 1, 0, false}),
        // test word [bx], 1
        ArithImmed16Params(std::vector<uint8_t>{0xf7, 0x0f, 0x01, 0x00},
                           {1, 1, 0, false})));

INSTANTIATE_TEST_CASE_P(Test,
                        ArithmeticAlImmedTest,
                        ::testing::Values(
                            // test al, 1
                            ArithImmed8Params(std::vector<uint8_t>{0xa8, 0x01},
                                              {1, 1, 0, false})));

INSTANTIATE_TEST_CASE_P(Test,
                        ArithmeticAxImmedTest,
                        ::testing::Values(
                            // test ax, 1
                            ArithImmed16Params(std::vector<uint8_t>{0xa9, 0x01,
                                                                    0x0},
                                               {1, 1, 0, false})));
