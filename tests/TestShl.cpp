#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

template <typename T>
struct ShlTest {
    T val;
    T expected;
    uint16_t flags;
    uint16_t expected_flags;
};

static const std::vector<struct ShlTest<uint8_t>> shl8_shift1_tests = {
    { 0, 0, 0, PF | ZF },
    { 1, 2, 0, 0 },
    { 0x80, 0x00, 0, CF | ZF | PF },
    { 0xc0, 0x80, 0, CF | SF | OF },
    { 0x40, 0x80, 0, SF },
};

TEST_F(EmulateFixture, ShlReg1)
{
    // shl al, 1
    for (auto &t: shl8_shift1_tests) {
        write_flags(t.flags);
        SCOPED_TRACE("shl " + std::to_string(static_cast<int>(t.val)) + " << 1");
        write_reg(AL, t.val);
        set_instruction({ 0xd0, 0xe0 });

        emulate();

        ASSERT_EQ(read_reg(AL), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_F(EmulateFixture, ShlMem1)
{
    // shl byte [bx], 1
    for (auto &t: shl8_shift1_tests) {
        write_flags(t.flags);
        SCOPED_TRACE("shl " + std::to_string(static_cast<int>(t.val)) + " << 1");
        write_reg(BX, 0x100);
        write_mem<uint8_t>(0x100, t.val);
        set_instruction({ 0xd0, 0x27 });

        emulate();

        ASSERT_EQ(read_mem<uint8_t>(0x100), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

static const std::vector<struct ShlTest<uint16_t>> shl16_shift1_tests = {
    { 0, 0, 0, PF | ZF },
    { 1, 2, 0, 0 },
    { 0x8000, 0x0000, 0, CF | ZF | PF },
    { 0xc000, 0x8000, 0, CF | SF | OF | PF },
    { 0x4000, 0x8000, 0, SF | PF },
};

TEST_F(EmulateFixture, ShlRegW1)
{
    // shl ax, 1
    for (auto &t: shl16_shift1_tests) {
        write_flags(t.flags);
        SCOPED_TRACE("shl " + std::to_string(static_cast<int>(t.val)) + " << 1");
        write_reg(AX, t.val);
        set_instruction({ 0xd1, 0xe0 });

        emulate();

        ASSERT_EQ(read_reg(AX), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_F(EmulateFixture, ShlMemW1)
{
    // shl word [bx], 1
    for (auto &t: shl16_shift1_tests) {
        write_flags(t.flags);
        SCOPED_TRACE("shl " + std::to_string(static_cast<int>(t.val)) + " << 1");
        write_reg(BX, 0x100);
        write_mem<uint16_t>(0x100, t.val);
        set_instruction({ 0xd1, 0x27 });

        emulate();

        ASSERT_EQ(read_mem<uint16_t>(0x100), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}
