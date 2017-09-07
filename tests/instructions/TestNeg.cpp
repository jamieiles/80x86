#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"
#include "Arithmetic.h"

// neg of numbers, 0 clears carry flag, others set it
// set AF CF OF PF SF ZF
// byte of -128 or word of -32768 causes no change but sets OF

template <typename T>
struct NegTest {
    T val;
    T expected;
    uint16_t expected_flags;
    bool carry_set;
};

using Neg8Params =
    std::pair<const std::vector<uint8_t>, const struct NegTest<uint8_t>>;
using Neg16Params =
    std::pair<const std::vector<uint8_t>, const struct NegTest<uint16_t>>;

static const std::vector<struct NegTest<uint8_t>> neg8_tests = {
    {0, 0, ZF | PF, false}, {0, 0, ZF | PF, true},
        {64, 0xc0, SF | CF | PF, false}, {0x80, 0x80, CF | SF | OF, false},
};

static const std::vector<struct NegTest<uint16_t>> neg16_tests = {
    {0, 0, ZF | PF, false}, {0, 0, ZF | PF, true},
        {256, 0xff00, SF | CF | PF, false},
};

TEST_F(EmulateFixture, Neg8Reg)
{
    // neg al
    for (auto &t : neg8_tests) {
        reset();

        SCOPED_TRACE("neg " + std::to_string(static_cast<int>(t.val)) +
                     " cf=" + std::to_string(static_cast<int>(!!t.carry_set)));
        write_reg(AL, t.val);
        write_flags(t.carry_set ? CF : 0);
        set_instruction({0xf6, 0xd8});

        emulate();

        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
        ASSERT_EQ(read_reg(AL), t.expected);
    }
}

TEST_F(EmulateFixture, Neg8Mem)
{
    // neg byte [bx]
    for (auto &t : neg8_tests) {
        reset();

        SCOPED_TRACE("neg " + std::to_string(static_cast<int>(t.val)) +
                     " cf=" + std::to_string(static_cast<int>(!!t.carry_set)));
        write_reg(BX, 0x0100);
        write_mem8(0x0100, t.val);
        write_flags(t.carry_set ? CF : 0);
        set_instruction({0xf6, 0x1f});

        emulate();

        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
        ASSERT_EQ(read_mem8(0x0100), t.expected);
    }
}

TEST_F(EmulateFixture, Neg16Reg)
{
    // neg ax
    for (auto &t : neg16_tests) {
        reset();

        SCOPED_TRACE("neg " + std::to_string(static_cast<int>(t.val)) +
                     " cf=" + std::to_string(static_cast<int>(!!t.carry_set)));
        write_reg(AX, t.val);
        write_flags(t.carry_set ? CF : 0);
        set_instruction({0xf7, 0xd8});

        emulate();

        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
        ASSERT_EQ(read_reg(AX), t.expected);
    }
}

TEST_F(EmulateFixture, Neg16Mem)
{
    // neg byte [bx]
    for (auto &t : neg16_tests) {
        reset();

        SCOPED_TRACE("neg " + std::to_string(static_cast<int>(t.val)) +
                     " cf=" + std::to_string(static_cast<int>(!!t.carry_set)));
        write_reg(BX, 0x0100);
        write_mem16(0x0100, t.val);
        write_flags(t.carry_set ? CF : 0);
        set_instruction({0xf7, 0x1f});

        emulate();

        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
        ASSERT_EQ(read_mem16(0x0100), t.expected);
    }
}
