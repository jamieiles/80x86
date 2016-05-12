#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"
#include "Arithmetic.h"

static const std::vector<struct ArithmeticTest<uint8_t>> sbb8_tests = {
    { 0, 0, 0, PF | ZF, false },
    { 0, 0, 0xff, PF | OF | SF | CF, true },
    { 0, 1, 0xff, PF | SF | OF | CF | AF, false },
    { 3, 2, 1, 0, false },
    { 3, 2, 0, PF | ZF, true },
    { 0, 1, 0xff, PF | SF | OF | CF | AF, false },
    { 0xff, 0xff, 0, PF | OF | ZF, false },
    { 0xff, 0xff, 0xff, PF | SF | CF, true },
    { 0, 0xff, 1, CF | AF, false },
};

static const std::vector<struct ArithmeticTest<uint16_t>> sbb16_tests = {
    { 0, 0, 0, PF | ZF, false },
    { 0, 0, 0xffff, PF | OF | SF | CF, true },
    { 0, 1, 0xffff, PF | SF | OF | CF | AF, false },
    { 3, 2, 1, 0, false },
    { 3, 2, 0, PF | ZF, true },
    { 0, 1, 0xffff, PF | SF | OF | CF | AF, false },
    { 0xffff, 0xffff, 0, PF | OF | ZF, false },
    { 0xffff, 0xffff, 0xffff, PF | SF | CF, true },
    { 0, 0xffff, 1, CF | AF, false },
};

INSTANTIATE_TEST_CASE_P(Sbb, ArithmeticRegReg8Test,
    ::testing::Values(
        // sbb al, bl
        Arith8Params({ 0x18, 0xd8 }, sbb8_tests)
    ));

INSTANTIATE_TEST_CASE_P(Sbb, ArithmeticMemReg8Test,
    ::testing::Values(
        // sbb [bx], al
        Arith8Params({ 0x18, 0x07 }, sbb8_tests)
    ));

INSTANTIATE_TEST_CASE_P(Sbb, ArithmeticRegReg8TestReversed,
    ::testing::Values(
        // sbb bl, al
        Arith8Params({ 0x1a, 0xd8 }, sbb8_tests)
    ));

INSTANTIATE_TEST_CASE_P(Sbb, ArithmeticMemReg8TestReversed,
    ::testing::Values(
        // sbb al, [bx]
        Arith8Params({ 0x1a, 0x07 }, sbb8_tests)
    ));

INSTANTIATE_TEST_CASE_P(Sbb, ArithmeticRegReg16Test,
    ::testing::Values(
        // sbb ax, bx
        Arith16Params({ 0x19, 0xd8 }, sbb16_tests)
    ));

INSTANTIATE_TEST_CASE_P(Sbb, ArithmeticRegMem16Test,
    ::testing::Values(
        // adc [bx], ax
        Arith16Params({ 0x19, 0x07 }, sbb16_tests)
    ));

INSTANTIATE_TEST_CASE_P(Sbb, ArithmeticRegReg16TestReversed,
    ::testing::Values(
        // sbb bx, ax
        Arith16Params({ 0x1b, 0xd8 }, sbb16_tests)
    ));

INSTANTIATE_TEST_CASE_P(Sbb, ArithmeticMemReg16TestReversed,
    ::testing::Values(
        // sbb ax, bx
        Arith16Params({ 0x1b, 0x07 }, sbb16_tests)
    ));

// Immediates

INSTANTIATE_TEST_CASE_P(Sbb, ArithmeticRegImmed8Test,
    ::testing::Values(
        // sbb bl, 1
        ArithImmed8Params(
            std::vector<uint8_t>{ 0x80, 0xdb, 0x01 },
            { 2, 1, 0, false }
        ),
        ArithImmed8Params(
            std::vector<uint8_t>{ 0x82, 0xdb, 0x01 },
            { 2, 1, 0, false }
        )
    ));

INSTANTIATE_TEST_CASE_P(Sbb, ArithmeticMemImmed8Test,
    ::testing::Values(
        // sbb byte [bx], 1
        ArithImmed8Params(
            std::vector<uint8_t>{ 0x80, 0x1f, 0x01 },
            { 2, 1, 0, false }
        ),
        ArithImmed8Params(
            std::vector<uint8_t>{ 0x82, 0x1f, 0x01 },
            { 2, 1, 0, false }
        )
    ));

INSTANTIATE_TEST_CASE_P(Sbb, ArithmeticRegImmed16Test,
    ::testing::Values(
        // sbb bx, 1
        ArithImmed16Params(
            std::vector<uint8_t>{ 0x81, 0xdb, 0x01, 0x0 },
            { 2, 1, 0, false }
        )
    ));

INSTANTIATE_TEST_CASE_P(Sbb, ArithmeticMemImmed16Test,
    ::testing::Values(
        // sbb word [bx], 1
        ArithImmed16Params(
            std::vector<uint8_t>{ 0x81, 0x1f, 0x01, 0x00 },
            { 2, 1, 0, false }
        )
    ));

INSTANTIATE_TEST_CASE_P(Sbb, ArithmeticRegImmed16TestExtend,
    ::testing::Values(
        // sbb bx, -1
        ArithImmed16Params(
            std::vector<uint8_t>{ 0x83, 0xdb, 0xff },
            { 1, 2, CF | AF, false }
        ),
        // sbb bx, 1
        ArithImmed16Params(
            std::vector<uint8_t>{ 0x83, 0xdb, 0x01 },
            { 2, 1, 0, false }
        )
    ));

INSTANTIATE_TEST_CASE_P(Sbb, ArithmeticMemImmed16TestExtend,
    ::testing::Values(
        // sbb word [bx], -1
        ArithImmed16Params(
            std::vector<uint8_t>{ 0x83, 0x1f, 0xff },
            { 1, 2, CF | AF, false }
        ),
        // sbb word [bx], 1
        ArithImmed16Params(
            std::vector<uint8_t>{ 0x83, 0x1f, 0x01 },
            { 2, 1, 0, false }
        )
    ));

INSTANTIATE_TEST_CASE_P(Sbb, ArithmeticAlImmedTest,
    ::testing::Values(
        // sbb al, 1
        ArithImmed8Params(
            std::vector<uint8_t>{ 0x1c, 0x01 },
            { 2, 1, 0, false }
        ),
        ArithImmed8Params(
            std::vector<uint8_t>{ 0x1c, 0x01 },
            { 3, 1, 0, true }
        )
    ));

INSTANTIATE_TEST_CASE_P(Sbb, ArithmeticAxImmedTest,
    ::testing::Values(
        // sbb ax, 1
        ArithImmed16Params(
            std::vector<uint8_t>{ 0x1d, 0x01, 0x0 },
            { 2, 1, 0, false }
        ),
        ArithImmed16Params(
            std::vector<uint8_t>{ 0x1d, 0x01, 0x0 },
            { 3, 1, 0, true }
        )
    ));
