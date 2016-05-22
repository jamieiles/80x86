#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"
#include "Shift.h"

static const std::vector<struct ShiftTest<uint8_t>> shl8_shift1_tests = {
    { 1, 0, 0, 0, PF | ZF },
    { 1, 1, 2, 0, 0 },
    { 1, 0x80, 0x00, 0, CF | ZF | PF },
    { 1, 0xc0, 0x80, 0, CF | SF | OF },
    { 1, 0x40, 0x80, 0, SF },
};

static const std::vector<struct ShiftTest<uint16_t>> shl16_shift1_tests = {
    { 1, 0, 0, 0, PF | ZF },
    { 1, 1, 2, 0, 0 },
    { 1, 0x8000, 0x0000, 0, CF | ZF | PF },
    { 1, 0xc000, 0x8000, 0, CF | SF | OF | PF },
    { 1, 0x4000, 0x8000, 0, SF | PF },
};

INSTANTIATE_TEST_CASE_P(Shl, ShiftReg8Test,
    ::testing::Values(
        // shl al, 1
        Shift8Params({ 0xd0, 0xe0 }, shl8_shift1_tests)
    ));

INSTANTIATE_TEST_CASE_P(Shl, ShiftMem8Test,
    ::testing::Values(
        // shl byte [bx], 1
        Shift8Params({ 0xd0, 0x27 }, shl8_shift1_tests)
    ));

INSTANTIATE_TEST_CASE_P(Shl, ShiftReg16Test,
    ::testing::Values(
        // shl ax, 1
        Shift16Params({ 0xd1, 0xe0 }, shl16_shift1_tests)
    ));

INSTANTIATE_TEST_CASE_P(Shl, ShiftMem16Test,
    ::testing::Values(
        // shl word [bx], 1
        Shift16Params({ 0xd1, 0x27 }, shl16_shift1_tests)
    ));
