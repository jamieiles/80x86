#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"
#include "Shift.h"

static const std::vector<struct ShiftTest<uint8_t>> rcr8_shift1_tests = {
    { 1, 0, 0, 0, 0 },
    { 1, 0x80, 0x40, 0, OF },
    { 1, 0, 0x80, CF, OF },
    { 1, 2, 1, 0, 0 },
    { 1, 0x80, 0xc0, CF, 0 },
};

static const std::vector<struct ShiftTest<uint16_t>> rcr16_shift1_tests = {
    { 1, 0, 0, 0, 0 },
    { 1, 0x8000, 0x4000, 0, OF },
    { 1, 0, 0x8000, CF, OF },
    { 1, 2, 1, 0, 0 },
    { 1, 0x8000, 0xc000, CF, 0 },
};

INSTANTIATE_TEST_CASE_P(Rcrg1, ShiftReg8Test,
    ::testing::Values(
        // rcr al, 1
        Shift8Params({ 0xd0, 0xd8 }, rcr8_shift1_tests)
    ));

INSTANTIATE_TEST_CASE_P(Rcrg1, ShiftMem8Test,
    ::testing::Values(
        // rcr byte [bx], 1
        Shift8Params({ 0xd0, 0x1f }, rcr8_shift1_tests)
    ));

INSTANTIATE_TEST_CASE_P(Rcrg1, ShiftReg16Test,
    ::testing::Values(
        // rcr ax, 1
        Shift16Params({ 0xd1, 0xd8 }, rcr16_shift1_tests)
    ));

INSTANTIATE_TEST_CASE_P(Rcrg1, ShiftMem16Test,
    ::testing::Values(
        // rcr word [bx], 1
        Shift16Params({ 0xd1, 0x1f }, rcr16_shift1_tests)
    ));

static const std::vector<struct ShiftTest<uint8_t>> rcr8_shiftN_tests = {
    { 1, 0, 0, 0, 0 },
    { 1, 0x80, 0x40, 0, 0 },
    { 1, 0, 0x80, CF, 0 },
    { 1, 2, 1, 0, 0 },
    { 1, 0x80, 0xc0, CF, 0 },
    { 8, 0, 0, 0, 0 },
    { 7, 0x80, 0x1, 0, 0 },
    { 8, 0x80, 0x00, 0, CF },
};

static const std::vector<struct ShiftTest<uint16_t>> rcr16_shiftN_tests = {
    { 1, 0, 0, 0, 0 },
    { 1, 0x8000, 0x4000, 0, 0 },
    { 1, 0, 0x8000, CF, 0 },
    { 1, 2, 1, 0, 0 },
    { 1, 0x8000, 0xc000, CF, 0 },
    { 16, 0, 0, 0, 0 },
    { 15, 0x8000, 0x1, 0, 0 },
    { 16, 0x8000, 0x00, 0, CF },
};

INSTANTIATE_TEST_CASE_P(RcrgN, ShiftReg8Test,
    ::testing::Values(
        // rcr al, N
        Shift8Params({ 0xd2, 0xd8 }, rcr8_shiftN_tests)
    ));

INSTANTIATE_TEST_CASE_P(RcrgN, ShiftMem8Test,
    ::testing::Values(
        // rcr byte [bx], N
        Shift8Params({ 0xd2, 0x1f }, rcr8_shiftN_tests)
    ));

INSTANTIATE_TEST_CASE_P(RcrgN, ShiftReg16Test,
    ::testing::Values(
        // rcr ax, N
        Shift16Params({ 0xd3, 0xd8 }, rcr16_shiftN_tests)
    ));

INSTANTIATE_TEST_CASE_P(RcrgN, ShiftMem16Test,
    ::testing::Values(
        // rcr word [bx], N
        Shift16Params({ 0xd3, 0x1f }, rcr16_shiftN_tests)
    ));

INSTANTIATE_TEST_CASE_P(RcrgCL, ShiftCLTest,
    ::testing::Values(
        ShiftCLTestParams{4, 0x40, { 0xd2, 0xd1 }}
    ));
