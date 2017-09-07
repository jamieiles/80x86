#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"
#include "Shift.h"

static const std::vector<struct ShiftTest<uint8_t>> shr8_shift1_tests = {
    {1, 0, 0, 0, PF | ZF}, {1, 1, 0, 0, PF | ZF | CF}, {1, 2, 1, 0, 0},
        {1, 0, 0, CF, PF | ZF},
};

static const std::vector<struct ShiftTest<uint16_t>> shr16_shift1_tests = {
    {1, 0, 0, 0, PF | ZF}, {1, 1, 0, 0, PF | ZF | CF}, {1, 2, 1, 0, 0},
        {1, 0, 0, CF, PF | ZF},
};

INSTANTIATE_TEST_CASE_P(Shr1,
                        ShiftReg8Test,
                        ::testing::Values(
                            // shr al, 1
                            Shift8Params({0xd0, 0xe8}, shr8_shift1_tests)));

INSTANTIATE_TEST_CASE_P(Shr1,
                        ShiftMem8Test,
                        ::testing::Values(
                            // shr byte [bx], 1
                            Shift8Params({0xd0, 0x2f}, shr8_shift1_tests)));

INSTANTIATE_TEST_CASE_P(Shr1,
                        ShiftReg16Test,
                        ::testing::Values(
                            // shr ax, 1
                            Shift16Params({0xd1, 0xe8}, shr16_shift1_tests)));

INSTANTIATE_TEST_CASE_P(Shr1,
                        ShiftMem16Test,
                        ::testing::Values(
                            // shr word [bx], 1
                            Shift16Params({0xd1, 0x2f}, shr16_shift1_tests)));

static const std::vector<struct ShiftTest<uint8_t>> shr8_shiftN_tests = {
    {0, 1, 1, 0, 0}, {1, 0, 0, 0, PF | ZF}, {1, 1, 0, 0, PF | ZF | CF},
        {1, 2, 1, 0, 0}, {1, 0, 0, CF, PF | ZF}, {8, 0, 0, 0, PF | ZF},
        {7, 0x80, 1, 0, 0}, {8, 0x80, 0x00, 0, CF | PF | ZF},
};

static const std::vector<struct ShiftTest<uint16_t>> shr16_shiftN_tests = {
    {0, 1, 1, 0, 0}, {1, 0, 0, 0, PF | ZF}, {1, 1, 0, 0, PF | ZF | CF},
        {1, 2, 1, 0, 0}, {1, 0, 0, CF, PF | ZF}, {8, 0, 0, 0, PF | ZF},
        {15, 0x8000, 1, 0, 0}, {16, 0x8000, 0x00, 0, CF | PF | ZF},
};

INSTANTIATE_TEST_CASE_P(ShrN,
                        ShiftReg8Test,
                        ::testing::Values(
                            // shr al, N
                            Shift8Params({0xd2, 0xe8}, shr8_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(ShrN,
                        ShiftMem8Test,
                        ::testing::Values(
                            // shr byte [bx], N
                            Shift8Params({0xd2, 0x2f}, shr8_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(ShrN,
                        ShiftReg16Test,
                        ::testing::Values(
                            // shr ax, N
                            Shift16Params({0xd3, 0xe8}, shr16_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(ShrN,
                        ShiftMem16Test,
                        ::testing::Values(
                            // shr word [bx], N
                            Shift16Params({0xd3, 0x2f}, shr16_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(ShrN,
                        ShiftRegImm8Test,
                        ::testing::Values(
                            // shr ax, imm8
                            Shift8Params({0xc0, 0xe8}, shr8_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(ShrN,
                        ShiftMemImm8Test,
                        ::testing::Values(
                            // shr word [bx], imm8
                            Shift8Params({0xc0, 0x2f}, shr8_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(ShrN,
                        ShiftRegImm16Test,
                        ::testing::Values(
                            // shr ax, imm16
                            Shift16Params({0xc1, 0xe8}, shr16_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(ShrN,
                        ShiftMemImm16Test,
                        ::testing::Values(
                            // shr word [bx], imm16
                            Shift16Params({0xc1, 0x2f}, shr16_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(
    ShrCL,
    ShiftCLTest,
    ::testing::Values(ShiftCLTestParams{4, 0, {0xd2, 0xe9}}));
