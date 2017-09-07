#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "Arithmetic.h"
#include "Flags.h"

static const std::vector<struct ArithmeticTest<uint8_t>> adc8_tests = {
    {0, 0, 0, PF | ZF, false}, {0, 0, 1, 0, true}, {0xf, 1, 0x10, AF, false},
        {255, 0, 0, ZF | AF | CF | PF, true}, {0xff, 0, 0xff, PF | SF, false},
        {127, 0, 128, OF | SF | AF, true}, {255, 1, 1, CF | AF, true},
};

static const std::vector<struct ArithmeticTest<uint16_t>> adc16_tests = {
    {0, 0, 0, PF | ZF, false}, {0, 1, 2, 0, true}, {0xf, 1, 0x10, AF, false},
        {0xffff, 0, 0, ZF | AF | CF | PF, true},
        {0xffff, 0, 0xffff, PF | SF, false},
        {32767, 0, 32768, OF | SF | AF | PF, true},
        {0xffff, 1, 1, CF | AF, true},
        {0x7fff, 1, 0x8000, SF | PF | AF | OF, false},
};

INSTANTIATE_TEST_CASE_P(Adc,
                        ArithmeticRegReg8Test,
                        ::testing::Values(
                            // adc al, bl
                            Arith8Params({0x10, 0xd8}, adc8_tests)));

INSTANTIATE_TEST_CASE_P(Adc,
                        ArithmeticMemReg8Test,
                        ::testing::Values(
                            // adc [bx], al
                            Arith8Params({0x10, 0x07}, adc8_tests)));

INSTANTIATE_TEST_CASE_P(Adc,
                        ArithmeticRegReg8TestReversed,
                        ::testing::Values(
                            // adc bl, al
                            Arith8Params({0x12, 0xd8}, adc8_tests)));

INSTANTIATE_TEST_CASE_P(Adc,
                        ArithmeticMemReg8TestReversed,
                        ::testing::Values(
                            // adc al, [bx]
                            Arith8Params({0x12, 0x07}, adc8_tests)));

INSTANTIATE_TEST_CASE_P(Adc,
                        ArithmeticRegReg16Test,
                        ::testing::Values(
                            // adc ax, bx
                            Arith16Params({0x11, 0xd8}, adc16_tests)));

INSTANTIATE_TEST_CASE_P(Adc,
                        ArithmeticRegMem16Test,
                        ::testing::Values(
                            // adc [bx], ax
                            Arith16Params({0x11, 0x07}, adc16_tests)));

INSTANTIATE_TEST_CASE_P(Adc,
                        ArithmeticRegReg16TestReversed,
                        ::testing::Values(
                            // adc bx, ax
                            Arith16Params({0x13, 0xd8}, adc16_tests)));

INSTANTIATE_TEST_CASE_P(Adc,
                        ArithmeticMemReg16TestReversed,
                        ::testing::Values(
                            // adc ax, bx
                            Arith16Params({0x13, 0x07}, adc16_tests)));

INSTANTIATE_TEST_CASE_P(
    Adc,
    ArithmeticRegImmed8Test,
    ::testing::Values(
        // adc bl, 1
        ArithImmed8Params(std::vector<uint8_t>{0x80, 0xd3, 0x01},
                          {1, 2, 0, false}),
        ArithImmed8Params(std::vector<uint8_t>{0x82, 0xd3, 0x01},
                          {1, 2, 0, false}),
        ArithImmed8Params(std::vector<uint8_t>{0x80, 0xd3, 0x01},
                          {1, 3, PF, true}),
        ArithImmed8Params(std::vector<uint8_t>{0x82, 0xd3, 0x01},
                          {1, 3, PF, true})));

INSTANTIATE_TEST_CASE_P(
    Adc,
    ArithmeticMemImmed8Test,
    ::testing::Values(
        // adc byte [bx], 1
        ArithImmed8Params(std::vector<uint8_t>{0x80, 0x17, 0x01},
                          {1, 2, 0, false}),
        ArithImmed8Params(std::vector<uint8_t>{0x82, 0x17, 0x01},
                          {1, 2, 0, false}),
        ArithImmed8Params(std::vector<uint8_t>{0x80, 0x17, 0x01},
                          {1, 3, PF, true}),
        ArithImmed8Params(std::vector<uint8_t>{0x82, 0x17, 0x01},
                          {1, 3, PF, true})));

INSTANTIATE_TEST_CASE_P(
    Adc,
    ArithmeticRegImmed16Test,
    ::testing::Values(
        // adc bx, 1
        ArithImmed16Params(std::vector<uint8_t>{0x81, 0xd3, 0x01, 0x0},
                           {1, 2, 0, false}),
        ArithImmed16Params(std::vector<uint8_t>{0x81, 0xd3, 0x01, 0x0},
                           {1, 3, PF, true})));

INSTANTIATE_TEST_CASE_P(
    Adc,
    ArithmeticMemImmed16Test,
    ::testing::Values(
        // adc word [bx], 1
        ArithImmed16Params(std::vector<uint8_t>{0x81, 0x17, 0x01, 0x00},
                           {1, 2, 0, false}),
        ArithImmed16Params(std::vector<uint8_t>{0x81, 0x17, 0x01, 0x00},
                           {1, 3, PF, true})));

INSTANTIATE_TEST_CASE_P(
    Adc,
    ArithmeticRegImmed16TestExtend,
    ::testing::Values(
        // adc bx, -1
        ArithImmed16Params(std::vector<uint8_t>{0x83, 0xd3, 0xff},
                           {2, 1, CF | AF, false}),
        // adc bx, 1
        ArithImmed16Params(std::vector<uint8_t>{0x83, 0xd3, 0x01},
                           {2, 3, PF, false})));

INSTANTIATE_TEST_CASE_P(
    Adc,
    ArithmeticMemImmed16TestExtend,
    ::testing::Values(
        // adc word [bx], -1
        ArithImmed16Params(std::vector<uint8_t>{0x83, 0x17, 0xff},
                           {2, 1, CF | AF, false}),
        // adc word [bx], 1
        ArithImmed16Params(std::vector<uint8_t>{0x83, 0x17, 0x01},
                           {2, 3, PF, false})));

INSTANTIATE_TEST_CASE_P(Adc,
                        ArithmeticAlImmedTest,
                        ::testing::Values(
                            // adc al, 1
                            ArithImmed8Params(std::vector<uint8_t>{0x14, 0x01},
                                              {1, 2, 0, false})));

INSTANTIATE_TEST_CASE_P(Adc,
                        ArithmeticAxImmedTest,
                        ::testing::Values(
                            // adc ax, 1
                            ArithImmed16Params(std::vector<uint8_t>{0x15, 0x01,
                                                                    0x0},
                                               {1, 2, 0, false})));
