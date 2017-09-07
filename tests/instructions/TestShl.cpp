#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"
#include "Shift.h"

static const std::vector<struct ShiftTest<uint8_t>> shl8_shift1_tests = {
    {1, 0, 0, 0, PF | ZF}, {1, 1, 2, 0, 0},
        {1, 0x80, 0x00, 0, CF | ZF | OF | PF}, {1, 0xc0, 0x80, 0, CF | SF},
        {1, 0x40, 0x80, 0, SF | OF},
};

static const std::vector<struct ShiftTest<uint16_t>> shl16_shift1_tests = {
    {1, 0, 0, 0, PF | ZF}, {1, 1, 2, 0, 0},
        {1, 0x8000, 0x0000, 0, CF | ZF | OF | PF},
        {1, 0xc000, 0x8000, 0, CF | SF | PF},
        {1, 0x4000, 0x8000, 0, SF | PF | OF},
};

INSTANTIATE_TEST_CASE_P(Shl1,
                        ShiftReg8Test,
                        ::testing::Values(
                            // shl al, 1
                            Shift8Params({0xd0, 0xe0}, shl8_shift1_tests),
                            // sal al, 1
                            Shift8Params({0xd0, 0xf0}, shl8_shift1_tests)));

INSTANTIATE_TEST_CASE_P(Shl1,
                        ShiftMem8Test,
                        ::testing::Values(
                            // shl byte [bx], 1
                            Shift8Params({0xd0, 0x27}, shl8_shift1_tests),
                            // sal byte [bx], 1
                            Shift8Params({0xd0, 0x37}, shl8_shift1_tests)));

INSTANTIATE_TEST_CASE_P(Shl1,
                        ShiftReg16Test,
                        ::testing::Values(
                            // shl ax, 1
                            Shift16Params({0xd1, 0xe0}, shl16_shift1_tests),
                            // sal ax, 1
                            Shift16Params({0xd1, 0xf0}, shl16_shift1_tests)));

INSTANTIATE_TEST_CASE_P(Shl1,
                        ShiftMem16Test,
                        ::testing::Values(
                            // shl word [bx], 1
                            Shift16Params({0xd1, 0x27}, shl16_shift1_tests),
                            // sal word [bx], 1
                            Shift16Params({0xd1, 0x37}, shl16_shift1_tests)));

static const std::vector<struct ShiftTest<uint8_t>> shl8_shiftN_tests = {
    {0, 1, 1, 0, 0}, {1, 0, 0, 0, PF | ZF}, {1, 1, 2, 0, 0},
        {1, 0x80, 0x00, 0, CF | ZF | PF}, {1, 0xc0, 0x80, 0, CF | SF},
        {1, 0x40, 0x80, 0, SF}, {8, 0, 0, 0, PF | ZF}, {7, 1, 0x80, 0, SF},
        {8, 1, 0x00, 0, CF | PF | ZF},
};

static const std::vector<struct ShiftTest<uint16_t>> shl16_shiftN_tests = {
    {0, 1, 1, 0, 0}, {1, 0, 0, 0, PF | ZF}, {1, 1, 2, 0, 0},
        {1, 0x8000, 0x0000, 0, CF | ZF | PF},
        {1, 0xc000, 0x8000, 0, CF | SF | PF}, {1, 0x4000, 0x8000, 0, SF | PF},
        {8, 0, 0, 0, PF | ZF}, {15, 1, 0x8000, 0, SF | PF},
        {16, 1, 0x0000, 0, CF | PF | ZF},
};

INSTANTIATE_TEST_CASE_P(ShlN,
                        ShiftReg8Test,
                        ::testing::Values(
                            // shl al, N
                            Shift8Params({0xd2, 0xe0}, shl8_shiftN_tests),
                            // sal al, N
                            Shift8Params({0xd2, 0xf0}, shl8_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(ShlN,
                        ShiftMem8Test,
                        ::testing::Values(
                            // shl byte [bx], N
                            Shift8Params({0xd2, 0x27}, shl8_shiftN_tests),
                            // sal byte [bx], N
                            Shift8Params({0xd2, 0x37}, shl8_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(ShlN,
                        ShiftReg16Test,
                        ::testing::Values(
                            // shl ax, N
                            Shift16Params({0xd3, 0xe0}, shl16_shiftN_tests),
                            // sal ax, N
                            Shift16Params({0xd3, 0xf0}, shl16_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(ShlN,
                        ShiftMem16Test,
                        ::testing::Values(
                            // shl word [bx], N
                            Shift16Params({0xd3, 0x27}, shl16_shiftN_tests),
                            // sal word [bx], N
                            Shift16Params({0xd3, 0x37}, shl16_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(ShlN,
                        ShiftRegImm8Test,
                        ::testing::Values(
                            // shl ax, imm8
                            Shift8Params({0xc0, 0xe0}, shl8_shiftN_tests),
                            // sal ax, imm8
                            Shift8Params({0xc0, 0xf0}, shl8_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(ShlN,
                        ShiftMemImm8Test,
                        ::testing::Values(
                            // shl word [bx], imm8
                            Shift8Params({0xc0, 0x27}, shl8_shiftN_tests),
                            // sal word [bx], imm8
                            Shift8Params({0xc0, 0x37}, shl8_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(ShlN,
                        ShiftRegImm16Test,
                        ::testing::Values(
                            // shl ax, imm16
                            Shift16Params({0xc1, 0xe0}, shl16_shiftN_tests),
                            // sal ax, imm16
                            Shift16Params({0xc1, 0xf0}, shl16_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(ShlN,
                        ShiftMemImm16Test,
                        ::testing::Values(
                            // shl word [bx], imm16
                            Shift16Params({0xc1, 0x27}, shl16_shiftN_tests),
                            // sal word [bx], imm16
                            Shift16Params({0xc1, 0x37}, shl16_shiftN_tests)));

INSTANTIATE_TEST_CASE_P(
    ShlCL,
    ShiftCLTest,
    ::testing::Values(ShiftCLTestParams{4, 64, {0xd2, 0xe1}}));
