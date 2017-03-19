#include "Shift.h"
#include "EmulateFixture.h"
#include "Flags.h"

TEST_P(ShiftReg8Test, ResultAndFlags)
{
    for (auto &t: GetParam().second) {
        reset();

        write_flags(t.flags);
        write_reg(CL, t.shift_count);
        SCOPED_TRACE("SHIFT " + std::to_string(static_cast<int>(t.val)) +
                     " << " + std::to_string(static_cast<int>(t.shift_count)) +
                     ((t.flags & CF) ? " + CF" : ""));
        write_reg(AL, t.val);
        set_instruction(GetParam().first);

        emulate();

        ASSERT_EQ(read_reg(AL), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_P(ShiftMem8Test, ResultAndFlags)
{
    for (auto &t: GetParam().second) {
        reset();

        write_flags(t.flags);
        write_reg(CL, t.shift_count);
        SCOPED_TRACE("SHIFT " + std::to_string(static_cast<int>(t.val)) +
                     " << " + std::to_string(static_cast<int>(t.shift_count)) +
                     ((t.flags & CF) ? " + CF" : ""));
        write_reg(BX, 0x100);
        write_mem<uint8_t>(0x100, t.val);
        set_instruction(GetParam().first);

        emulate();

        ASSERT_EQ(read_mem<uint8_t>(0x100), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_P(ShiftReg16Test, ResultAndFlags)
{
    // shl ax, COUNT
    for (auto &t: GetParam().second) {
        reset();

        write_flags(t.flags);
        write_reg(CL, t.shift_count);
        SCOPED_TRACE("SHIFT " + std::to_string(static_cast<int>(t.val)) +
                     " << " + std::to_string(static_cast<int>(t.shift_count)) +
                     ((t.flags & CF) ? " + CF" : ""));
        write_reg(AX, t.val);
        set_instruction(GetParam().first);

        emulate();

        ASSERT_EQ(read_reg(AX), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

TEST_P(ShiftMem16Test, ResultAndFlags)
{
    for (auto &t: GetParam().second) {
        reset();

        write_flags(t.flags);
        write_reg(CL, t.shift_count);
        SCOPED_TRACE("SHIFT " + std::to_string(static_cast<int>(t.val)) +
                     " << " + std::to_string(static_cast<int>(t.shift_count)) +
                     ((t.flags & CF) ? " + CF" : ""));
        write_reg(BX, 0x100);
        write_mem<uint16_t>(0x100, t.val);
        set_instruction(GetParam().first);

        emulate();

        ASSERT_EQ(read_mem<uint16_t>(0x100), t.expected);
        ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                            FLAGS_STUCK_BITS | t.expected_flags);
    }
}

// Shifting the count is an odd thing to do, but we should shift the initial
// value even though it is decremented on each iteration.
TEST_P(ShiftCLTest, ShiftCL)
{
    write_reg(CL, GetParam().count);
    set_instruction(GetParam().instruction);

    emulate();

    ASSERT_EQ(GetParam().expected, read_reg(CL));
}
