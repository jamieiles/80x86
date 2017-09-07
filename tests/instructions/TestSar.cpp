#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"
#include "Shift.h"

static const std::vector<struct ShiftTest<uint8_t>> sar8_shift1_tests = {
    {1, 0, 0, 0, PF | ZF}, {1, 1, 0, 0, PF | ZF | CF}, {1, 2, 1, 0, 0},
        {1, 0, 0, CF, PF | ZF}, {1, 0x80, 0xc0, CF, PF | SF},
        {1, 0x80, 0xc0, 0, PF | SF},
};

static const std::vector<struct ShiftTest<uint16_t>> sar16_shift1_tests = {
    {1, 0, 0, 0, PF | ZF}, {1, 1, 0, 0, PF | ZF | CF}, {1, 2, 1, 0, 0},
        {1, 0, 0, CF, PF | ZF}, {1, 0x8000, 0xc000, CF, PF | SF},
        {1, 0x8000, 0xc000, 0, PF | SF},
};

INSTANTIATE_TEST_CASE_P(Sar1,
                        ShiftReg8Test,
                        ::testing::Values(
                            // sar al, 1
                            Shift8Params({0xd0, 0xf8}, sar8_shift1_tests)));

INSTANTIATE_TEST_CASE_P(Sar1,
                        ShiftMem8Test,
                        ::testing::Values(
                            // sar byte [bx], 1
                            Shift8Params({0xd0, 0x3f}, sar8_shift1_tests)));

INSTANTIATE_TEST_CASE_P(Sar1,
                        ShiftReg16Test,
                        ::testing::Values(
                            // sar ax, 1
                            Shift16Params({0xd1, 0xf8}, sar16_shift1_tests)));

INSTANTIATE_TEST_CASE_P(Sar1,
                        ShiftMem16Test,
                        ::testing::Values(
                            // sar word [bx], 1
                            Shift16Params({0xd1, 0x3f}, sar16_shift1_tests)));

static const std::vector<struct ShiftTest<uint8_t>> sar8_shiftN_tests = {
    {0, 1, 1, 0, 0}, {1, 0, 0, 0, PF | ZF}, {1, 1, 0, 0, PF | ZF | CF},
        {1, 2, 1, 0, 0}, {1, 0, 0, CF, PF | ZF}, {1, 0x80, 0xc0, CF, PF | SF},
        {1, 0x80, 0xc0, 0, PF | SF}, {8, 0, 0, 0, PF | ZF},
        {7, 0x80, 0xff, 0, PF | SF},
    {
        8, 0x80, 0xff, 0, PF | SF | CF
    }
};

static const std::vector<struct ShiftTest<uint16_t>> sar16_shiftN_tests = {
    {0, 1, 1, 0, 0}, {1, 0, 0, 0, PF | ZF}, {1, 1, 0, 0, PF | ZF | CF},
        {1, 2, 1, 0, 0}, {1, 0, 0, CF, PF | ZF},
        {1, 0x8000, 0xc000, CF, PF | SF}, {1, 0x8000, 0xc000, 0, PF | SF},
        {16, 0, 0, 0, PF | ZF}, {15, 0x8000, 0xffff, 0, PF | SF},
    {
        16, 0x8000, 0xffff, 0, PF | SF | CF
    }
};

INSTANTIATE_TEST_CASE_P(SarN,
                        ShiftReg8Test,
                        ::testing::Values(
                            // sar al, N
                            Shift8Params({0xd2, 0xf8}, sar8_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(SarN,
                        ShiftMem8Test,
                        ::testing::Values(
                            // sar byte [bx], N
                            Shift8Params({0xd2, 0x3f}, sar8_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(SarN,
                        ShiftReg16Test,
                        ::testing::Values(
                            // sar ax, N
                            Shift16Params({0xd3, 0xf8}, sar16_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(SarN,
                        ShiftMem16Test,
                        ::testing::Values(
                            // sar word [bx], N
                            Shift16Params({0xd3, 0x3f}, sar16_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(SarN,
                        ShiftRegImm8Test,
                        ::testing::Values(
                            // sar ax, imm8
                            Shift8Params({0xc0, 0xf8}, sar8_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(SarN,
                        ShiftMemImm8Test,
                        ::testing::Values(
                            // sar word [bx], imm8
                            Shift8Params({0xc0, 0x3f}, sar8_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(SarN,
                        ShiftRegImm16Test,
                        ::testing::Values(
                            // sar ax, imm16
                            Shift16Params({0xc1, 0xf8}, sar16_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(SarN,
                        ShiftMemImm16Test,
                        ::testing::Values(
                            // sar word [bx], imm16
                            Shift16Params({0xc1, 0x3f}, sar16_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(
    SarCL,
    ShiftCLTest,
    ::testing::Values(ShiftCLTestParams{4, 0, {0xd2, 0xf9}}));
