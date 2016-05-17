#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"
#include "Arithmetic.h"

static const std::vector<struct ArithmeticTest<uint8_t>> add8_tests = {
    { 0, 0, 0, PF | ZF, false },
    { 0xf, 1, 0x10, AF, false },
    { 255, 1, 0, ZF | CF | PF | AF, false },
    { 0xff, 0, 0xff, PF | SF, false },
    { 127, 1, 128, OF | SF | AF, false },
};

static const std::vector<struct ArithmeticTest<uint16_t>> add16_tests = {
    { 0, 0, 0, PF | ZF, false },
    { 0xf, 1, 0x10, AF, false },
    { 0xffff, 1, 0, ZF | CF | PF | AF, false },
    { 0xffff, 0, 0xffff, PF | SF, false },
    { 32767, 1, 32768, OF | SF | AF | PF, false },
};

INSTANTIATE_TEST_CASE_P(Add, ArithmeticRegReg8Test,
    ::testing::Values(
        // add al, bl
        Arith8Params({ 0x00, 0xd8 }, add8_tests)
    ));

INSTANTIATE_TEST_CASE_P(Add, ArithmeticMemReg8Test,
    ::testing::Values(
        // add [bx], al
        Arith8Params({ 0x00, 0x07 }, add8_tests)
    ));

INSTANTIATE_TEST_CASE_P(Add, ArithmeticRegReg8TestReversed,
    ::testing::Values(
        // add bl, al
        Arith8Params({ 0x02, 0xd8 }, add8_tests)
    ));

INSTANTIATE_TEST_CASE_P(Add, ArithmeticMemReg8TestReversed,
    ::testing::Values(
        // add al, [bx]
        Arith8Params({ 0x02, 0x07 }, add8_tests)
    ));

INSTANTIATE_TEST_CASE_P(Add, ArithmeticRegReg16Test,
    ::testing::Values(
        // add ax, bx
        Arith16Params({ 0x01, 0xd8 }, add16_tests)
    ));

INSTANTIATE_TEST_CASE_P(Add, ArithmeticRegMem16Test,
    ::testing::Values(
        // adc [bx], ax
        Arith16Params({ 0x01, 0x07 }, add16_tests)
    ));

INSTANTIATE_TEST_CASE_P(Add, ArithmeticRegReg16TestReversed,
    ::testing::Values(
        // add bx, ax
        Arith16Params({ 0x03, 0xd8 }, add16_tests)
    ));

INSTANTIATE_TEST_CASE_P(Add, ArithmeticMemReg16TestReversed,
    ::testing::Values(
        // add ax, bx
        Arith16Params({ 0x03, 0x07 }, add16_tests)
    ));

INSTANTIATE_TEST_CASE_P(Add, ArithmeticRegImmed8Test,
    ::testing::Values(
        // add bl, 1
        ArithImmed8Params(
            std::vector<uint8_t>{ 0x80, 0xc3, 0x01 },
            { 1, 2, 0, false }
        ),
        ArithImmed8Params(
            std::vector<uint8_t>{ 0x82, 0xc3, 0x01 },
            { 1, 2, 0, false }
        )
    ));

INSTANTIATE_TEST_CASE_P(Add, ArithmeticMemImmed8Test,
    ::testing::Values(
        // add byte [bx], 1
        ArithImmed8Params(
            std::vector<uint8_t>{ 0x80, 0x07, 0x01 },
            { 1, 2, 0, false }
        ),
        ArithImmed8Params(
            std::vector<uint8_t>{ 0x82, 0x07, 0x01 },
            { 1, 2, 0, false }
        )
    ));

INSTANTIATE_TEST_CASE_P(Add, ArithmeticRegImmed16Test,
    ::testing::Values(
        // add bx, 1
        ArithImmed16Params(
            std::vector<uint8_t>{ 0x81, 0xc3, 0x01, 0x0 },
            { 1, 2, 0, false }
        )
    ));

INSTANTIATE_TEST_CASE_P(Add, ArithmeticMemImmed16Test,
    ::testing::Values(
        // add word [bx], 1
        ArithImmed16Params(
            std::vector<uint8_t>{ 0x81, 0x07, 0x01, 0x00 },
            { 1, 2, 0, false }
        )
    ));

INSTANTIATE_TEST_CASE_P(Add, ArithmeticRegImmed16TestExtend,
    ::testing::Values(
        // add bx, -1
        ArithImmed16Params(
            std::vector<uint8_t>{ 0x83, 0xc3, 0xff },
            { 2, 1, CF | AF, false }
        ),
        // add bx, 1
        ArithImmed16Params(
            std::vector<uint8_t>{ 0x83, 0xc3, 0x01 },
            { 2, 3, PF, false }
        )
    ));

INSTANTIATE_TEST_CASE_P(Add, ArithmeticMemImmed16TestExtend,
    ::testing::Values(
        // add word [bx], -1
        ArithImmed16Params(
            std::vector<uint8_t>{ 0x83, 0x10, 0xff },
            { 2, 1, CF | AF, false }
        ),
        // add word [bx], 1
        ArithImmed16Params(
            std::vector<uint8_t>{ 0x83, 0x10, 0x01 },
            { 2, 3, PF, false }
        )
    ));

INSTANTIATE_TEST_CASE_P(Add, ArithmeticAlImmedTest,
    ::testing::Values(
        // add al, 1
        ArithImmed8Params(
            std::vector<uint8_t>{ 0x04, 0x01 },
            { 1, 2, 0, false }
        )
    ));

INSTANTIATE_TEST_CASE_P(Add, ArithmeticAxImmedTest,
    ::testing::Values(
        // add ax, 1
        ArithImmed16Params(
            std::vector<uint8_t>{ 0x05, 0x01, 0x0 },
            { 1, 2, 0, false }
        )
    ));

TEST_F(EmulateFixture, MultipleAdd)
{
    // add al, 1; add al, 1
    set_instruction({ 0x04, 0x01, 0x04, 0x01 });

    emulate(2);

    ASSERT_EQ(0x2, read_reg(AL));
}
