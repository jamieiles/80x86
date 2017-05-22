#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"
#include "Shift.h"

static const std::vector<struct ShiftTest<uint8_t>> rol8_shift1_tests = {
    { 1, 0, 0, 0, 0 },
    { 1, 1, 2, 0, 0 },
    { 1, 0x80, 0x01, 0, CF | OF },
    { 1, 0xc0, 0x81, 0, CF },
    { 1, 0x40, 0x80, 0, OF },
};

static const std::vector<struct ShiftTest<uint16_t>> rol16_shift1_tests = {
    { 1, 0, 0, 0, 0 },
    { 1, 1, 2, 0, 0 },
    { 1, 0x8000, 0x0001, 0, CF | OF },
    { 1, 0xc000, 0x8001, 0, CF },
    { 1, 0x4000, 0x8000, 0, OF },
};

INSTANTIATE_TEST_CASE_P(Rol1, ShiftReg8Test,
    ::testing::Values(
        // rol al, 1
        Shift8Params({ 0xd0, 0xc0 }, rol8_shift1_tests)
    ));

INSTANTIATE_TEST_CASE_P(Rol1, ShiftMem8Test,
    ::testing::Values(
        // rol byte [bx], 1
        Shift8Params({ 0xd0, 0x07 }, rol8_shift1_tests)
    ));

INSTANTIATE_TEST_CASE_P(Rol1, ShiftReg16Test,
    ::testing::Values(
        // rol ax, 1
        Shift16Params({ 0xd1, 0xc0 }, rol16_shift1_tests)
    ));

INSTANTIATE_TEST_CASE_P(Rol1, ShiftMem16Test,
    ::testing::Values(
        // rol word [bx], 1
        Shift16Params({ 0xd1, 0x07 }, rol16_shift1_tests)
    ));

static const std::vector<struct ShiftTest<uint8_t>> rol8_shiftN_tests = {
    { 0, 1, 1, 0, 0 },
    { 1, 0, 0, 0, 0 },
    { 1, 1, 2, 0, 0 },
    { 1, 0x80, 0x01, 0, CF },
    { 1, 0xc0, 0x81, 0, CF },
    { 1, 0x40, 0x80, 0, 0 },
    { 8, 0, 0, 0, 0 },
    { 7, 1, 0x80, 0, 0 },
    { 8, 1, 0x01, 0, CF },
};

static const std::vector<struct ShiftTest<uint16_t>> rol16_shiftN_tests = {
    { 0, 1, 1, 0, 0 },
    { 1, 0, 0, 0, 0 },
    { 1, 1, 2, 0, 0 },
    { 1, 0x8000, 0x0001, 0, CF },
    { 1, 0xc000, 0x8001, 0, CF },
    { 1, 0x4000, 0x8000, 0, 0 },
    { 16, 0, 0, 0, 0 },
    { 15, 1, 0x8000, 0, 0 },
    { 16, 1, 0x0001, 0, CF },
};

INSTANTIATE_TEST_CASE_P(RolN, ShiftReg8Test,
    ::testing::Values(
        // rol al, N
        Shift8Params({ 0xd2, 0xc0 }, rol8_shiftN_tests)
    ));

INSTANTIATE_TEST_CASE_P(RolN, ShiftMem8Test,
    ::testing::Values(
        // rol byte [bx], N
        Shift8Params({ 0xd2, 0x07 }, rol8_shiftN_tests)
    ));

INSTANTIATE_TEST_CASE_P(RolN, ShiftReg16Test,
    ::testing::Values(
        // rol ax, N
        Shift16Params({ 0xd3, 0xc0 }, rol16_shiftN_tests)
    ));

INSTANTIATE_TEST_CASE_P(RolN, ShiftMem16Test,
    ::testing::Values(
        // rol word [bx], N
        Shift16Params({ 0xd3, 0x07 }, rol16_shiftN_tests)
    ));

INSTANTIATE_TEST_CASE_P(RolN, ShiftRegImm8Test,
    ::testing::Values(
        // rol ax, imm8
        Shift8Params({ 0xc0, 0xc0}, rol8_shiftN_tests)
    ));

INSTANTIATE_TEST_CASE_P(RolN, ShiftMemImm8Test,
    ::testing::Values(
        // rol word [bx], imm8
        Shift8Params({ 0xc0, 0x07}, rol8_shiftN_tests)
    ));

INSTANTIATE_TEST_CASE_P(RolN, ShiftRegImm16Test,
    ::testing::Values(
        // rol ax, imm16
        Shift16Params({ 0xc1, 0xc0}, rol16_shiftN_tests)
    ));

INSTANTIATE_TEST_CASE_P(RolN, ShiftMemImm16Test,
    ::testing::Values(
        // rol word [bx], imm16
        Shift16Params({ 0xc1, 0x07}, rol16_shiftN_tests)
    ));

INSTANTIATE_TEST_CASE_P(RolCL, ShiftCLTest,
    ::testing::Values(
        ShiftCLTestParams{4, 64, { 0xd2, 0xc1 }}
    ));
