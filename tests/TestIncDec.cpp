#include <stdint.h>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"
#include "Arithmetic.h"

template <typename T>
struct IncDecTest {
    T v;
    T expected;
    uint16_t expected_flags;
    bool carry_set;
};

using IncDec8Params = std::pair<const std::vector<uint8_t>,
      const std::vector<struct IncDecTest<uint8_t>>>;
using IncDec16Params = std::pair<const std::vector<uint8_t>,
      const std::vector<struct IncDecTest<uint16_t>>>;

static std::vector<IncDecTest<uint8_t>> inc8_tests = {
    { 0, 1, 0, false },
    { 0, 1, CF, true },
    { 0xff, 0, PF | ZF | AF, false },
    { 0xff, 0, PF | ZF | AF | CF, true },
};

static std::vector<IncDecTest<uint16_t>> inc16_tests = {
    { 0, 1, 0, false },
    { 0, 1, CF, true },
    { 0x00ff, 0x0100, CF | AF, true },
    { 0xffff, 0, PF | ZF | AF, false },
    { 0xffff, 0, PF | ZF | AF | CF, true },
};

static std::vector<IncDecTest<uint8_t>> dec8_tests = {
    { 1, 0, ZF | PF, false },
    { 1, 0, ZF | PF | CF, true },
    { 0, 0xff, PF | AF | OF | SF, false },
    { 0, 0xff, PF | AF | CF | OF | SF, true },
};

static std::vector<IncDecTest<uint16_t>> dec16_tests = {
    { 1, 0, ZF | PF, false },
    { 1, 0, ZF | PF | CF, true },
    { 0x0100, 0x00ff, CF | AF | PF, true },
    { 0, 0xffff, PF | AF | OF | SF, false },
    { 0, 0xffff, PF | AF | CF | OF | SF, true },
};

class IncReg8Test : public EmulateFixture,
    public ::testing::WithParamInterface<IncDec8Params> {
};
TEST_P(IncReg8Test, ResultAndFlags)
{
    auto params = GetParam();
    for (auto &t: params.second) {
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(AL, t.v);
        set_instruction(params.first);

        emulate();

        ASSERT_EQ(read_reg(AL), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
};
INSTANTIATE_TEST_CASE_P(Inc, IncReg8Test,
    ::testing::Values(
        // inc al
        IncDec8Params({ 0xfe, 0xc0 }, inc8_tests)
    ));
INSTANTIATE_TEST_CASE_P(Dec, IncReg8Test,
    ::testing::Values(
        // dec al
        IncDec8Params({ 0xfe, 0xc8 }, dec8_tests)
    ));

class IncMem8Test : public EmulateFixture,
    public ::testing::WithParamInterface<IncDec8Params> {
};
TEST_P(IncMem8Test, ResultAndFlags)
{
    auto params = GetParam();
    for (auto &t: params.second) {
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(BX, 0x0100);
        write_mem<uint8_t>(0x0100, t.v);
        set_instruction(params.first);

        emulate();

        ASSERT_EQ(read_mem<uint8_t>(0x0100), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
};
INSTANTIATE_TEST_CASE_P(Inc, IncMem8Test,
    ::testing::Values(
        // inc byte [bx]
        IncDec8Params({ 0xfe, 0x07 }, inc8_tests)
    ));
INSTANTIATE_TEST_CASE_P(Dec, IncMem8Test,
    ::testing::Values(
        // dec byte [bx]
        IncDec8Params({ 0xfe, 0x0f }, dec8_tests)
    ));

TEST_F(EmulateFixture, IncFEInvalidReg)
{
    set_instruction({ 0xfe, 0xcc });

    emulate();

    ASSERT_EQ(0x0, read_reg(AL));
}

class IncReg16Test : public EmulateFixture,
    public ::testing::WithParamInterface<IncDec16Params> {
};
TEST_P(IncReg16Test, ResultAndFlags)
{
    auto params = GetParam();
    for (auto &t: params.second) {
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(AX, t.v);
        set_instruction(params.first);

        emulate();

        ASSERT_EQ(read_reg(AX), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
};
INSTANTIATE_TEST_CASE_P(Inc, IncReg16Test,
    ::testing::Values(
        // inc ax
        IncDec16Params({ 0xff, 0xc0 }, inc16_tests)
    ));

class IncMem16Test : public EmulateFixture,
    public ::testing::WithParamInterface<IncDec16Params> {
};
TEST_P(IncMem16Test, ResultAndFlags)
{
    auto params = GetParam();
    for (auto &t: params.second) {
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(BX, 0x0100);
        write_mem<uint16_t>(0x0100, t.v);
        set_instruction(params.first);

        emulate();

        ASSERT_EQ(read_mem<uint16_t>(0x0100), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
};
INSTANTIATE_TEST_CASE_P(Inc, IncMem16Test,
    ::testing::Values(
        // inc word [bx]
        IncDec16Params({ 0xff, 0x07 }, inc16_tests)
    ));
INSTANTIATE_TEST_CASE_P(Dec, IncMem16Test,
    ::testing::Values(
        // dec word [bx]
        IncDec16Params({ 0xff, 0x0f }, dec16_tests)
    ));

TEST_F(EmulateFixture, IncReg)
{
    // inc REG
    for (uint8_t i = 0; i < 8; ++i) {
        auto reg = static_cast<GPR>(static_cast<int>(AX) + i);
        write_reg(reg, 0x00ff);

        set_instruction({ static_cast<uint8_t>(0x40 + i) });
        emulate();

        ASSERT_EQ(0x0100, read_reg(reg));
    }
}

TEST_F(EmulateFixture, DecReg)
{
    // dec REG
    for (uint8_t i = 0; i < 8; ++i) {
        auto reg = static_cast<GPR>(static_cast<int>(AX) + i);
        write_reg(reg, 0x0100);

        set_instruction({ static_cast<uint8_t>(0x48 + i) });
        emulate();

        ASSERT_EQ(0x00ff, read_reg(reg));
    }
}
