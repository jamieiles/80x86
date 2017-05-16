#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"
#include "Arithmetic.h"

static const std::vector<struct ArithmeticTest<uint8_t>> xor8_tests = {
    { 0, 0, 0, PF | ZF, false },
    { 0xf, 0xf, 0x00, PF | ZF, false },
    { 0x0f, 0xf0, 0xff, PF | SF, false },
    { 0x0f, 0x01, 0x0e, 0, false },
};

static const std::vector<struct ArithmeticTest<uint16_t>> xor16_tests = {
    { 0, 0, 0, PF | ZF, false },
    { 0xf, 0xf, 0x00, PF | ZF, false },
    { 0x00ff, 0xff00, 0xffff, PF | SF, false },
    { 0x0f, 0x01, 0x0e, 0, false },
};

INSTANTIATE_TEST_CASE_P(Xor, ArithmeticRegReg8Test,
    ::testing::Values(
        // xor al, bl
        Arith8Params({ 0x30, 0xd8 }, xor8_tests)
    ));

INSTANTIATE_TEST_CASE_P(Xor, ArithmeticMemReg8Test,
    ::testing::Values(
        // xor [bx], al
        Arith8Params({ 0x30, 0x07 }, xor8_tests)
    ));

INSTANTIATE_TEST_CASE_P(Xor, ArithmeticRegReg8TestReversed,
    ::testing::Values(
        // xor bl, al
        Arith8Params({ 0x32, 0xd8 }, xor8_tests)
    ));

INSTANTIATE_TEST_CASE_P(Xor, ArithmeticMemReg8TestReversed,
    ::testing::Values(
        // xor al, [bx]
        Arith8Params({ 0x32, 0x07 }, xor8_tests)
    ));

INSTANTIATE_TEST_CASE_P(Xor, ArithmeticRegReg16Test,
    ::testing::Values(
        // xor ax, bx
        Arith16Params({ 0x31, 0xd8 }, xor16_tests)
    ));

INSTANTIATE_TEST_CASE_P(Xor, ArithmeticRegMem16Test,
    ::testing::Values(
        // adc [bx], ax
        Arith16Params({ 0x31, 0x07 }, xor16_tests)
    ));

INSTANTIATE_TEST_CASE_P(Xor, ArithmeticRegReg16TestReversed,
    ::testing::Values(
        // xor bx, ax
        Arith16Params({ 0x33, 0xd8 }, xor16_tests)
    ));

INSTANTIATE_TEST_CASE_P(Xor, ArithmeticMemReg16TestReversed,
    ::testing::Values(
        // xor ax, bx
        Arith16Params({ 0x33, 0x07 }, xor16_tests)
    ));

INSTANTIATE_TEST_CASE_P(Xor, ArithmeticRegImmed8Test,
    ::testing::Values(
        // xor bl, 1
        ArithImmed8Params(
            std::vector<uint8_t>{ 0x80, 0xf3, 0x01 },
            { 1, 0, ZF | PF, false }
        )
    ));

INSTANTIATE_TEST_CASE_P(Xor, ArithmeticMemImmed8Test,
    ::testing::Values(
        // xor byte [bx], 1
        ArithImmed8Params(
            std::vector<uint8_t>{ 0x80, 0x37, 0x01 },
            { 1, 0, ZF | PF, false }
        )
    ));

INSTANTIATE_TEST_CASE_P(Xor, ArithmeticRegImmed16Test,
    ::testing::Values(
        // xor bx, 1
        ArithImmed16Params(
            std::vector<uint8_t>{ 0x81, 0xf3, 0x01, 0x0 },
            { 1, 0, ZF | PF, false }
        )
    ));

INSTANTIATE_TEST_CASE_P(Xor, ArithmeticMemImmed16Test,
    ::testing::Values(
        // xor word [bx], 1
        ArithImmed16Params(
            std::vector<uint8_t>{ 0x81, 0x37, 0x01, 0x00 },
            { 1, 0, ZF | PF, false }
        )
    ));

INSTANTIATE_TEST_CASE_P(Xor, ArithmeticRegImmed16TestExtend,
    ::testing::Values(
        // xor bx, 1
        ArithImmed16Params(
            std::vector<uint8_t>{ 0x83, 0xf3, 0x01 },
            { 1, 0, ZF | PF, false }
        )
    ));

INSTANTIATE_TEST_CASE_P(Xor, ArithmeticMemImmed16TestExtend,
    ::testing::Values(
        // xor word [bx], 1
        ArithImmed16Params(
            std::vector<uint8_t>{ 0x83, 0x37, 0x01 },
            { 1, 0, ZF | PF, false }
        )
    ));

INSTANTIATE_TEST_CASE_P(Xor, ArithmeticAlImmedTest,
    ::testing::Values(
        // xor al, 1
        ArithImmed8Params(
            std::vector<uint8_t>{ 0x34, 0x01 },
            { 1, 0, ZF | PF, false }
        )
    ));

INSTANTIATE_TEST_CASE_P(Xor, ArithmeticAxImmedTest,
    ::testing::Values(
        // xor ax, 1
        ArithImmed16Params(
            std::vector<uint8_t>{ 0x35, 0x01, 0x0 },
            { 1, 0, ZF | PF, false }
        )
    ));
