#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"
#include "Shift.h"

static const std::vector<struct ShiftTest<uint8_t>> rcl8_shift1_tests = {
    { 1, 0, 0, 0, 0 },
    { 1, 0, 1, CF, 0 },
    { 1, 1, 2, 0, 0 },
    { 1, 0x80, 0x00, 0, CF | OF },
    { 1, 0x80, 0x01, CF, CF | OF },
    { 1, 0x80, 0x00, 0, CF | OF },
    { 1, 0xc0, 0x81, CF, CF },
};

static const std::vector<struct ShiftTest<uint16_t>> rcl16_shift1_tests = {
    { 1, 0, 0, 0, 0 },
    { 1, 0, 1, CF, 0 },
    { 1, 1, 2, 0, 0 },
    { 1, 0x8000, 0x0000, 0, CF | OF },
    { 1, 0x8000, 0x0001, CF, CF | OF },
    { 1, 0x8000, 0x0000, 0, CF | OF },
    { 1, 0xc000, 0x8001, CF, CF },
};

INSTANTIATE_TEST_CASE_P(Rclg1, ShiftReg8Test,
    ::testing::Values(
        // rcl al, 1
        Shift8Params({ 0xd0, 0xd0 }, rcl8_shift1_tests)
    ));

INSTANTIATE_TEST_CASE_P(Rclg1, ShiftMem8Test,
    ::testing::Values(
        // rcl byte [bx], 1
        Shift8Params({ 0xd0, 0x17 }, rcl8_shift1_tests)
    ));

INSTANTIATE_TEST_CASE_P(Rclg1, ShiftReg16Test,
    ::testing::Values(
        // rcl ax, 1
        Shift16Params({ 0xd1, 0xd0 }, rcl16_shift1_tests)
    ));

INSTANTIATE_TEST_CASE_P(Rclg1, ShiftMem16Test,
    ::testing::Values(
        // rcl word [bx], 1
        Shift16Params({ 0xd1, 0x17 }, rcl16_shift1_tests)
    ));

static const std::vector<struct ShiftTest<uint8_t>> rcl8_shiftN_tests = {
    { 1, 0, 0, 0, 0 },
    { 1, 0, 1, CF, 0 },
    { 1, 1, 2, 0, 0 },
    { 1, 0x80, 0x00, 0, CF },
    { 1, 0x80, 0x01, CF, CF },
    { 1, 0x80, 0x00, 0, CF },
    { 1, 0xc0, 0x81, CF, CF },
    { 8, 0, 0, 0, 0 },
    { 7, 1, 0x80, 0, 0 },
    { 8, 1, 0x00, 0, CF },
};

static const std::vector<struct ShiftTest<uint16_t>> rcl16_shiftN_tests = {
    { 1, 0, 0, 0, 0 },
    { 1, 0, 1, CF, 0 },
    { 1, 1, 2, 0, 0 },
    { 1, 0x8000, 0x0000, 0, CF },
    { 1, 0x8000, 0x0001, CF, CF },
    { 1, 0x8000, 0x0000, 0, CF },
    { 1, 0xc000, 0x8001, CF, CF },
    { 16, 0, 0, 0, 0 },
    { 16, 0, 0x8000, CF, 0 },
    { 15, 1, 0x8000, 0, 0 },
    { 16, 1, 0x0000, 0, CF },
};

INSTANTIATE_TEST_CASE_P(RclgN, ShiftReg8Test,
    ::testing::Values(
        // rcl al, N
        Shift8Params({ 0xd2, 0xd0 }, rcl8_shiftN_tests)
    ));

INSTANTIATE_TEST_CASE_P(RclgN, ShiftMem8Test,
    ::testing::Values(
        // rcl byte [bx], N
        Shift8Params({ 0xd2, 0x17 }, rcl8_shiftN_tests)
    ));

INSTANTIATE_TEST_CASE_P(RclgN, ShiftReg16Test,
    ::testing::Values(
        // rcl ax, N
        Shift16Params({ 0xd3, 0xd0 }, rcl16_shiftN_tests)
    ));

INSTANTIATE_TEST_CASE_P(RclgN, ShiftMem16Test,
    ::testing::Values(
        // rcl word [bx], N
        Shift16Params({ 0xd3, 0x17 }, rcl16_shiftN_tests)
    ));

INSTANTIATE_TEST_CASE_P(RclgCL, ShiftCLTest,
    ::testing::Values(
        ShiftCLTestParams{4, 64, { 0xd2, 0xc1 }}
    ));
