#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

template <typename T>
struct AdcTest {
    T v1;
    T v2;
    T expected;
    uint16_t expected_flags;
    uint16_t carry_set;
};

static const std::vector<struct AdcTest<uint8_t>> add8_tests = {
    { 0, 0, 0, PF | ZF, false },
    { 0, 0, 1, 0, true },
    { 0xf, 1, 0x10, AF, false },
    { 255, 0, 0, ZF | CF | PF, true },
    { 0xff, 0, 0xff, PF | SF, false },
    { 127, 0, 128, OF | SF, true },
    { 255, 1, 1, CF | AF, true },
};

TEST_F(EmulateFixture, AdcRegReg8)
{
    for (auto &t: add8_tests) {
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)) + " + " +
                     std::to_string(static_cast<int>(t.carry_set)));
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(AL, t.v1);
        write_reg(BL, t.v2);
        // adc al, bl
        set_instruction({ 0x10, 0xd8 });

        emulate();

        ASSERT_EQ(read_reg(AL), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            0x8000 | t.expected_flags);
    }
}

TEST_F(EmulateFixture, AdcMemReg8)
{
    for (auto &t: add8_tests) {
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)) + " + " +
                     std::to_string(static_cast<int>(t.carry_set)));
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(AL, t.v2);
        write_reg(BX, 0x100);
        write_mem<uint8_t>(0x100, t.v1);

        // adc [bx], al
        set_instruction({ 0x10, 0x07 });

        emulate();

        ASSERT_EQ(read_mem<uint8_t>(0x100), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            0x8000 | t.expected_flags);
    }
}

TEST_F(EmulateFixture, AdcRegReg8Reversed)
{
    for (auto &t: add8_tests) {
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)) + " + " +
                     std::to_string(static_cast<int>(t.carry_set)));
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(AL, t.v2);
        write_reg(BL, t.v1);
        // adc bl, al
        set_instruction({ 0x12, 0xd8 });

        emulate();

        ASSERT_EQ(read_reg(BL), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            0x8000 | t.expected_flags);
    }
}

TEST_F(EmulateFixture, AdcMemReg8Reversed)
{
    for (auto &t: add8_tests) {
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)) + " + " +
                     std::to_string(static_cast<int>(t.carry_set)));
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(AL, t.v2);
        write_reg(BX, 0x100);
        write_mem<uint8_t>(0x100, t.v1);

        // adc al, [bx]
        set_instruction({ 0x12, 0x07 });

        emulate();

        ASSERT_EQ(read_reg(AL), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            0x8000 | t.expected_flags);
    }
}

static const std::vector<struct AdcTest<uint16_t>> add16_tests = {
    { 0, 0, 0, PF | ZF, false },
    { 0, 1, 2, 0, true },
    { 0xf, 1, 0x10, AF, false },
    { 0xffff, 0, 0, ZF | CF | PF, true },
    { 0xffff, 0, 0xffff, PF | SF, false },
    { 32767, 0, 32768, OF | SF, true },
    { 0xffff, 1, 1, CF | AF, true },
};

TEST_F(EmulateFixture, AdcRegReg16)
{
    for (auto &t: add16_tests) {
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)) + " + " +
                     std::to_string(static_cast<int>(t.carry_set)));
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(AX, t.v1);
        write_reg(BX, t.v2);
        // adc ax, bx
        set_instruction({ 0x11, 0xd8 });

        emulate();

        ASSERT_EQ(read_reg(AX), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            0x8000 | t.expected_flags);
    }
}

TEST_F(EmulateFixture, AdcMemReg16)
{
    for (auto &t: add16_tests) {
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)) + " + " +
                     std::to_string(static_cast<int>(t.carry_set)));
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(AX, t.v2);
        write_reg(BX, 0x100);
        write_mem<uint16_t>(0x100, t.v1);

        // adc [bx], ax
        set_instruction({ 0x11, 0x07 });

        emulate();

        ASSERT_EQ(read_mem<uint16_t>(0x100), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            0x8000 | t.expected_flags);
    }
}

TEST_F(EmulateFixture, AdcRegReg16Reversed)
{
    for (auto &t: add16_tests) {
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)) + " + " +
                     std::to_string(static_cast<int>(t.carry_set)));
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(AX, t.v2);
        write_reg(BX, t.v1);
        // adc bx, ax
        set_instruction({ 0x13, 0xd8 });

        emulate();

        ASSERT_EQ(read_reg(BX), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            0x8000 | t.expected_flags);
    }
}

TEST_F(EmulateFixture, AdcMemReg16Reversed)
{
    for (auto &t: add16_tests) {
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)) + " + " +
                     std::to_string(static_cast<int>(t.carry_set)));
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(AX, t.v2);
        write_reg(BX, 0x100);
        write_mem<uint16_t>(0x100, t.v1);

        // adc ax, [bx]
        set_instruction({ 0x13, 0x07 });

        emulate();

        ASSERT_EQ(read_reg(AX), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            0x8000 | t.expected_flags);
    }
}

TEST_F(EmulateFixture, AdcRegImmed8)
{
    // adc bl, 1
    for (auto opc: std::vector<uint8_t>{ 0x80, 0x82 }) {
        set_instruction({ opc, 0xd3, 0x01 });
        write_reg(BL, 1);

        emulate();

        ASSERT_EQ(read_reg(BL), 2);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), 0x8000);
    }
}

TEST_F(EmulateFixture, AdcMemImmed8)
{
    // adc byte [bx], 1
    for (auto opc: std::vector<uint8_t>{ 0x80, 0x82 }) {
        set_instruction({ opc, 0x17, 0x01 });
        write_mem<uint8_t>(0x0100, 0x1);
        write_reg(BX, 0x0100);

        emulate();

        ASSERT_EQ(read_mem<uint8_t>(0x0100), 2);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), 0x8000);
    }
}

TEST_F(EmulateFixture, AdcRegImmed16)
{
    // adc bx, 1
    set_instruction({ 0x81, 0xd3, 0x01, 0x00 });
    write_reg(BX, 1);

    emulate();

    ASSERT_EQ(read_reg(BX), 2);
    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), 0x8000);
}

TEST_F(EmulateFixture, AdcMemImmed16)
{
    // adc word [bx], 1
    set_instruction({ 0x81, 0x17, 0x01, 0x00 });
    write_mem<uint16_t>(0x0100, 0x1);
    write_reg(BX, 0x0100);

    emulate();

    ASSERT_EQ(read_mem<uint16_t>(0x0100), 2);
    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), 0x8000);
}

TEST_F(EmulateFixture, AdcRegImmed16Extend)
{
    // adc bx, -1
    set_instruction({ 0x83, 0xd3, 0xff });
    write_reg(BX, 2);

    emulate();

    ASSERT_EQ(read_reg(BX), 1);
    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), 0x8000 | CF | AF);
}

TEST_F(EmulateFixture, AdcMemImmed16Extend)
{
    // adc word [bx], 1
    set_instruction({ 0x83, 0x17, 0xff });
    write_mem<uint16_t>(0x0100, 0x2);
    write_reg(BX, 0x0100);

    emulate();

    ASSERT_EQ(read_mem<uint16_t>(0x0100), 1);
    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), 0x8000 | CF | AF);
}

TEST_F(EmulateFixture, AdcAlImmed)
{
    // adc al, 1
    set_instruction({ 0x14, 0x01 });
    write_reg(AL, 1);

    emulate();

    ASSERT_EQ(read_reg(AL), 2);
    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), 0x8000);
}

TEST_F(EmulateFixture, AdcAxImmed)
{
    // adc ax, 1
    set_instruction({ 0x15, 0x01, 0x00 });
    write_reg(AX, 1);

    emulate();

    ASSERT_EQ(read_reg(AX), 2);
    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), 0x8000);
}
