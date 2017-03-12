#include "Arithmetic.h"
#include "Flags.h"

TEST_P(ArithmeticRegReg8Test, ResultAndFlags)
{
    auto params = GetParam();
    for (auto &t: params.second) {
        reset();
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)) + " + " +
                     std::to_string(static_cast<int>(t.carry_set)));
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(AL, t.v1);
        write_reg(BL, t.v2);
        // ARITH al, bl
        set_instruction(params.first);

        emulate();

        ASSERT_EQ(read_reg(AL), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_P(ArithmeticMemReg8Test, ResultAndFlags)
{
    auto params = GetParam();
    for (auto &t: params.second) {
        reset();
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)) + " + " +
                     std::to_string(static_cast<int>(t.carry_set)));
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(AL, t.v2);
        write_reg(BX, 0x100);
        write_mem<uint8_t>(0x100, t.v1);

        // ARITH [bx], al
        set_instruction(params.first);

        emulate();

        ASSERT_EQ(read_mem<uint8_t>(0x100), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_P(ArithmeticRegReg8TestReversed, ResultAndFlags)
{
    auto params = GetParam();
    for (auto &t: params.second) {
        reset();
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)) + " + " +
                     std::to_string(static_cast<int>(t.carry_set)));
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(AL, t.v2);
        write_reg(BL, t.v1);
        // ARITH bl, al
        set_instruction(params.first);

        emulate();

        ASSERT_EQ(read_reg(BL), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_P(ArithmeticMemReg8TestReversed, ResultAndFlags)
{
    auto params = GetParam();
    for (auto &t: params.second) {
        reset();
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)) + " + " +
                     std::to_string(static_cast<int>(t.carry_set)));
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(AL, t.v1);
        write_reg(BX, 0x100);
        write_mem<uint8_t>(0x100, t.v2);

        // ARITH al, [bx]
        set_instruction(params.first);

        emulate();

        ASSERT_EQ(read_reg(AL), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_P(ArithmeticRegReg8TestReversedNoResult, ResultAndFlags)
{
    auto params = GetParam();
    for (auto &t: params.second) {
        reset();
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)) + " + " +
                     std::to_string(static_cast<int>(t.carry_set)));
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(AL, t.v2);
        write_reg(BL, t.v1);
        // ARITH bl, al
        set_instruction(params.first);

        emulate();

        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_P(ArithmeticMemReg8TestReversedNoResult, ResultAndFlags)
{
    auto params = GetParam();
    for (auto &t: params.second) {
        reset();
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)) + " + " +
                     std::to_string(static_cast<int>(t.carry_set)));
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(AL, t.v1);
        write_reg(BX, 0x100);
        write_mem<uint8_t>(0x100, t.v2);

        // ARITH al, [bx]
        set_instruction(params.first);

        emulate();

        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_P(ArithmeticRegReg16Test, ResultAndFlags)
{
    auto params = GetParam();
    for (auto &t: params.second) {
        reset();
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)) + " + " +
                     std::to_string(static_cast<int>(t.carry_set)));
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(AX, t.v1);
        write_reg(BX, t.v2);
        // ARITH ax, bx
        set_instruction(params.first);

        emulate();

        ASSERT_EQ(read_reg(AX), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_P(ArithmeticRegMem16Test, ResultAndFlags)
{
    auto params = GetParam();
    for (auto &t: params.second) {
        reset();
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)) + " + " +
                     std::to_string(static_cast<int>(t.carry_set)));
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(AX, t.v2);
        write_reg(BX, 0x100);
        write_mem<uint16_t>(0x100, t.v1);

        // ARITH [bx], ax
        set_instruction(params.first);

        emulate();

        ASSERT_EQ(read_mem<uint16_t>(0x100), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_P(ArithmeticRegReg16TestReversed, ResultAndFlags)
{
    auto params = GetParam();
    for (auto &t: params.second) {
        reset();
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)) + " + " +
                     std::to_string(static_cast<int>(t.carry_set)));
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(AX, t.v2);
        write_reg(BX, t.v1);
        // ARITH bx, ax
        set_instruction(params.first);

        emulate();

        ASSERT_EQ(read_reg(BX), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_P(ArithmeticMemReg16TestReversed, ResultAndFlags)
{
    auto params = GetParam();
    for (auto &t: params.second) {
        reset();
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)) + " + " +
                     std::to_string(static_cast<int>(t.carry_set)));
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(AX, t.v1);
        write_reg(BX, 0x100);
        write_mem<uint16_t>(0x100, t.v2);

        // ARITH ax, [bx]
        set_instruction(params.first);

        emulate();

        ASSERT_EQ(read_reg(AX), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_P(ArithmeticRegReg16TestReversedNoResult, ResultAndFlags)
{
    auto params = GetParam();
    for (auto &t: params.second) {
        reset();
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)) + " + " +
                     std::to_string(static_cast<int>(t.carry_set)));
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(AX, t.v2);
        write_reg(BX, t.v1);
        // ARITH bx, ax
        set_instruction(params.first);

        emulate();

        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_P(ArithmeticMemReg16TestReversedNoResult, ResultAndFlags)
{
    auto params = GetParam();
    for (auto &t: params.second) {
        reset();
        SCOPED_TRACE(std::to_string(static_cast<int>(t.v1)) + " + " +
                     std::to_string(static_cast<int>(t.v2)) + " + " +
                     std::to_string(static_cast<int>(t.carry_set)));
        write_flags(0);
        if (t.carry_set)
            write_flags(CF);
        write_reg(AX, t.v1);
        write_reg(BX, 0x100);
        write_mem<uint16_t>(0x100, t.v2);

        // ARITH ax, [bx]
        set_instruction(params.first);

        emulate();

        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_P(ArithmeticRegImmed8Test, ResultAndFlags)
{
    auto params = GetParam();
    if (params.second.carry_set)
        write_flags(CF);
    set_instruction(params.first);
    write_reg(BL, params.second.v1);

    emulate();

    ASSERT_EQ(read_reg(BL), params.second.expected);
    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | params.second.expected_flags);
}

TEST_P(ArithmeticMemImmed8Test, ResultAndFlags)
{
    auto params = GetParam();
    if (params.second.carry_set)
        write_flags(CF);
    set_instruction(params.first);
    write_mem<uint8_t>(0x0100, params.second.v1);
    write_reg(BX, 0x0100);

    emulate();

    ASSERT_EQ(read_mem<uint8_t>(0x0100), params.second.expected);
    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | params.second.expected_flags);
}

TEST_P(ArithmeticRegImmed16Test, ResultAndFlags)
{
    auto params = GetParam();
    if (params.second.carry_set)
        write_flags(CF);
    // ARITH bx, 1
    set_instruction(params.first);
    write_reg(BX, params.second.v1);

    emulate();

    ASSERT_EQ(read_reg(BX), params.second.expected);
    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | params.second.expected_flags);
}

TEST_P(ArithmeticMemImmed16Test, ResultAndFlags)
{
    auto params = GetParam();
    if (params.second.carry_set)
        write_flags(CF);
    // ARITH word [bx], 1
    set_instruction(params.first);
    write_mem<uint16_t>(0x0100, params.second.v1);
    write_reg(BX, 0x0100);

    emulate();

    ASSERT_EQ(read_mem<uint16_t>(0x0100), params.second.expected);
    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | params.second.expected_flags);
}

TEST_P(ArithmeticRegImmed16TestExtend, ResultAndFlags)
{
    auto params = GetParam();
    if (params.second.carry_set)
        write_flags(CF);
    // ARITH bx, -1
    set_instruction(params.first);
    write_reg(BX, params.second.v1);

    emulate();

    ASSERT_EQ(read_reg(BX), params.second.expected);
    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | params.second.expected_flags);
}

TEST_P(ArithmeticMemImmed16TestExtend, ResultAndFlags)
{
    auto params = GetParam();
    if (params.second.carry_set)
        write_flags(CF);
    // ARITH word [bx], 1
    set_instruction(params.first);
    write_mem<uint16_t>(0x0100, params.second.v1);
    write_reg(BX, 0x0100);

    emulate();

    ASSERT_EQ(read_mem<uint16_t>(0x0100), params.second.expected);
    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | params.second.expected_flags);
}

TEST_P(ArithmeticAlImmedTest, ResultAndFlags)
{
    auto params = GetParam();
    if (params.second.carry_set)
        write_flags(CF);
    // ARITH al, 1
    set_instruction(params.first);
    write_reg(AL, params.second.v1);

    emulate();

    ASSERT_EQ(read_reg(AL), params.second.expected);
    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | params.second.expected_flags);
}

TEST_P(ArithmeticAxImmedTest, ResultAndFlags)
{
    auto params = GetParam();
    if (params.second.carry_set)
        write_flags(CF);
    // ARITH ax, 1
    set_instruction(params.first);
    write_reg(AX, params.second.v1);

    emulate();

    ASSERT_EQ(read_reg(AX), params.second.expected);
    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | params.second.expected_flags);
}
