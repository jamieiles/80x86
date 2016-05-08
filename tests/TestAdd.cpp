#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"

template <typename T>
struct AddTest {
    T v1;
    T v2;
    T expected;
    uint16_t expected_flags;
};

static std::string flags_to_string(uint16_t flags)
{
    std::stringstream ss;

    if (flags & CF)
        ss << "CF ";
    if (flags & PF)
        ss << "PF ";
    if (flags & AF)
        ss << "AF ";
    if (flags & ZF)
        ss << "ZF ";
    if (flags & SF)
        ss << "SF ";
    if (flags & TF)
        ss << "TF ";
    if (flags & IF)
        ss << "IF ";
    if (flags & DF)
        ss << "DF ";
    if (flags & OF)
        ss << "OF ";

    return ss.str();
}

::testing::AssertionResult AssertFlagsEqual(const char *m_expr,
                                            const char *n_expr,
                                            uint16_t m,
                                            uint16_t n)
{
    if (m == n)
        return ::testing::AssertionSuccess();

    auto m_repr = flags_to_string(m);
    auto n_repr = flags_to_string(n);

    return ::testing::AssertionFailure()
        << m_expr << " and " << n_expr << " are not equal "
        << "m(" << m_repr << ") " << "n(" << n_repr << ")";
}

static const std::vector<struct AddTest<uint8_t>> add8_tests = {
    { 0, 0, 0, PF | ZF },
    { 0xf, 1, 0x10, AF },
    { 255, 1, 0, ZF | CF | PF | AF },
    { 0xff, 0, 0xff, PF | SF },
    { 127, 1, 128, OF | SF | AF },
};

TEST_F(EmulateFixture, AddRegReg8)
{
    for (auto &t: add8_tests) {
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)));
        write_reg(AL, t.v1);
        write_reg(BL, t.v2);
        write_flags(0);
        // add al, bl
        set_instruction({ 0x00, 0xd8 });

        emulate();

        ASSERT_EQ(read_reg(AL), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            0x8000 | t.expected_flags);
    }
}

TEST_F(EmulateFixture, AddMemReg8)
{
    for (auto &t: add8_tests) {
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)));
        write_reg(AL, t.v2);
        write_reg(BX, 0x100);
        write_mem<uint8_t>(0x100, t.v1);

        write_flags(0);
        // add [bx], al
        set_instruction({ 0x00, 0x07 });

        emulate();

        ASSERT_EQ(read_mem<uint8_t>(0x100), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            0x8000 | t.expected_flags);
    }
}

TEST_F(EmulateFixture, AddRegReg8Reversed)
{
    for (auto &t: add8_tests) {
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)));
        write_reg(AL, t.v2);
        write_reg(BL, t.v1);
        write_flags(0);
        // add bl, al
        set_instruction({ 0x02, 0xd8 });

        emulate();

        ASSERT_EQ(read_reg(BL), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            0x8000 | t.expected_flags);
    }
}

TEST_F(EmulateFixture, AddMemReg8Reversed)
{
    for (auto &t: add8_tests) {
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)));
        write_reg(AL, t.v2);
        write_reg(BX, 0x100);
        write_mem<uint8_t>(0x100, t.v1);

        write_flags(0);
        // add al, [bx]
        set_instruction({ 0x02, 0x07 });

        emulate();

        ASSERT_EQ(read_reg(AL), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            0x8000 | t.expected_flags);
    }
}

static const std::vector<struct AddTest<uint16_t>> add16_tests = {
    { 0, 0, 0, PF | ZF },
    { 0xf, 1, 0x10, AF },
    { 0xffff, 1, 0, ZF | CF | PF | AF },
    { 0xffff, 0, 0xffff, PF | SF },
    { 32767, 1, 32768, OF | SF | AF },
};

TEST_F(EmulateFixture, AddRegReg16)
{
    for (auto &t: add16_tests) {
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)));
        write_reg(AX, t.v1);
        write_reg(BX, t.v2);
        write_flags(0);
        // add ax, bx
        set_instruction({ 0x01, 0xd8 });

        emulate();

        ASSERT_EQ(read_reg(AX), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            0x8000 | t.expected_flags);
    }
}

TEST_F(EmulateFixture, AddMemReg16)
{
    for (auto &t: add16_tests) {
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)));
        write_reg(AX, t.v2);
        write_reg(BX, 0x100);
        write_mem<uint16_t>(0x100, t.v1);

        write_flags(0);
        // add [bx], ax
        set_instruction({ 0x01, 0x07 });

        emulate();

        ASSERT_EQ(read_mem<uint16_t>(0x100), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            0x8000 | t.expected_flags);
    }
}

TEST_F(EmulateFixture, AddRegReg16Reversed)
{
    for (auto &t: add16_tests) {
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)));
        write_reg(AX, t.v2);
        write_reg(BX, t.v1);
        write_flags(0);
        // add bx, ax
        set_instruction({ 0x03, 0xd8 });

        emulate();

        ASSERT_EQ(read_reg(BX), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            0x8000 | t.expected_flags);
    }
}

TEST_F(EmulateFixture, AddMemReg16Reversed)
{
    for (auto &t: add16_tests) {
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)));
        write_reg(AX, t.v2);
        write_reg(BX, 0x100);
        write_mem<uint16_t>(0x100, t.v1);

        write_flags(0);
        // add ax, [bx]
        set_instruction({ 0x03, 0x07 });

        emulate();

        ASSERT_EQ(read_reg(AX), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            0x8000 | t.expected_flags);
    }
}
