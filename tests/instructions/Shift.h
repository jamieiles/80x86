#pragma once

#include <vector>
#include <gtest/gtest.h>
#include "EmulateFixture.h"

template <typename T>
struct ShiftTest {
    int shift_count;
    T val;
    T expected;
    uint16_t flags;
    uint16_t expected_flags;
};

using Shift8Params = std::pair<const std::vector<uint8_t>,
      const std::vector<struct ShiftTest<uint8_t>>>;
using Shift16Params = std::pair<const std::vector<uint8_t>,
      const std::vector<struct ShiftTest<uint16_t>>>;

class ShiftReg8Test : public EmulateFixture,
    public ::testing::WithParamInterface<Shift8Params> {
};

class ShiftMem8Test : public EmulateFixture,
    public ::testing::WithParamInterface<Shift8Params> {
};

class ShiftRegImm8Test : public EmulateFixture,
    public ::testing::WithParamInterface<Shift8Params> {
};

class ShiftMemImm8Test : public EmulateFixture,
    public ::testing::WithParamInterface<Shift8Params> {
};

class ShiftReg16Test : public EmulateFixture,
    public ::testing::WithParamInterface<Shift16Params> {
};

class ShiftMem16Test : public EmulateFixture,
    public ::testing::WithParamInterface<Shift16Params> {
};

class ShiftRegImm16Test : public EmulateFixture,
    public ::testing::WithParamInterface<Shift16Params> {
};

class ShiftMemImm16Test : public EmulateFixture,
    public ::testing::WithParamInterface<Shift16Params> {
};

struct ShiftCLTestParams {
    uint8_t count;
    uint8_t expected;
    std::vector<uint8_t> instruction;
};

class ShiftCLTest : public EmulateFixture,
    public ::testing::WithParamInterface<ShiftCLTestParams> {
};
