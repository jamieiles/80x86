#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"
#include "Arithmetic.h"

template <typename T>
struct NotTest {
    T val;
    T expected;
};

using Not8Params = std::pair<const std::vector<uint8_t>,
      const struct NotTest<uint8_t>>;
using Not16Params = std::pair<const std::vector<uint8_t>,
      const struct NotTest<uint16_t>>;

static const std::vector<struct NotTest<uint8_t>> not8_tests = {
    { 0x00, 0xff },
    { 0xff, 0x00 },
    { 0xaa, 0x55 },
};

static const std::vector<struct NotTest<uint16_t>> not16_tests = {
    { 0xaa55, 0x55aa },
    { 0x0000, 0xffff },
    { 0xffff, 0x0000 },
};

TEST_F(EmulateFixture, Not8Reg)
{
    // not al
    for (auto &t: not8_tests) {
        reset();

        SCOPED_TRACE("not " + std::to_string(static_cast<int>(t.val)));
        write_reg(AL, t.val);
        set_instruction({ 0xf6, 0xd0 });

        emulate();

        ASSERT_EQ(read_reg(AL), t.expected);
    }
}

TEST_F(EmulateFixture, Not8Mem)
{
    // not byte [bx]
    for (auto &t: not8_tests) {
        reset();

        SCOPED_TRACE("not " + std::to_string(static_cast<int>(t.val)));
        write_reg(BX, 0x0100);
        write_mem8(0x0100, t.val);
        set_instruction({ 0xf6, 0x17 });

        emulate();

        ASSERT_EQ(read_mem8(0x0100), t.expected);
    }
}

TEST_F(EmulateFixture, Not16Reg)
{
    // not ax
    for (auto &t: not16_tests) {
        reset();

        SCOPED_TRACE("not " + std::to_string(static_cast<int>(t.val)));
        write_reg(AX, t.val);
        set_instruction({ 0xf7, 0xd0 });

        emulate();

        ASSERT_EQ(read_reg(AX), t.expected);
    }
}

TEST_F(EmulateFixture, Not16Mem)
{
    // not byte [bx]
    for (auto &t: not16_tests) {
        reset();

        SCOPED_TRACE("not " + std::to_string(static_cast<int>(t.val)));
        write_reg(BX, 0x0100);
        write_mem16(0x0100, t.val);
        set_instruction({ 0xf7, 0x17 });

        emulate();

        ASSERT_EQ(read_mem16(0x0100), t.expected);
    }
}
