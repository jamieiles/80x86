#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

// flags
// cs
// ip
TEST_F(EmulateFixture, Int3)
{
    // int 3
    write_reg(SP, 0x100);
    write_reg(CS, 0x7c00);
    write_reg(IP, 0x0001);
    write_flags(CF | IF | TF);

    write_mem<uint16_t>(VEC_INT + 2, 0x8000); // CS
    write_mem<uint16_t>(VEC_INT + 0, 0x0100); // IP

    set_instruction({ 0xcc });
    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | CF);
    ASSERT_EQ(read_reg(CS), 0x8000);
    ASSERT_EQ(read_reg(IP), 0x0100);
    ASSERT_EQ(read_reg(SP), 0x0100 - 6);

    ASSERT_EQ(read_mem<uint16_t>(0x100 - 2), FLAGS_STUCK_BITS | CF | IF | TF);
    ASSERT_EQ(read_mem<uint16_t>(0x100 - 4), 0x7c00);
    // Return to the following instruction
    ASSERT_EQ(read_mem<uint16_t>(0x100 - 6), 0x0002);
}

TEST_F(EmulateFixture, IntN)
{
    // int 3
    write_reg(SP, 0x100);
    write_reg(CS, 0x7c00);
    write_reg(IP, 0x0001);
    write_flags(CF | IF | TF);

    write_mem<uint16_t>(VEC_INT + 2, 0x8000); // CS
    write_mem<uint16_t>(VEC_INT + 0, 0x0100); // IP

    set_instruction({ 0xcd, 0x03 });
    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | CF);
    ASSERT_EQ(read_reg(CS), 0x8000);
    ASSERT_EQ(read_reg(IP), 0x0100);
    ASSERT_EQ(read_reg(SP), 0x0100 - 6);

    ASSERT_EQ(read_mem<uint16_t>(0x100 - 2), FLAGS_STUCK_BITS | CF | IF | TF);
    ASSERT_EQ(read_mem<uint16_t>(0x100 - 4), 0x7c00);
    // Return to the following instruction
    ASSERT_EQ(read_mem<uint16_t>(0x100 - 6), 0x0003);
}

TEST_F(EmulateFixture, IntoNotTaken)
{
    // int 3
    write_reg(SP, 0x100);
    write_reg(CS, 0x7c00);
    write_reg(IP, 0x0001);
    write_flags(CF | IF | TF);

    set_instruction({ 0xce });
    emulate();

    ASSERT_EQ(read_reg(IP), 0x0002);
    ASSERT_EQ(read_reg(SP), 0x0100);
}

TEST_F(EmulateFixture, IntoTaken)
{
    // int 3
    write_reg(SP, 0x100);
    write_reg(CS, 0x7c00);
    write_reg(IP, 0x0001);
    write_flags(CF | IF | TF | OF);

    write_mem<uint16_t>(VEC_OVERFLOW + 2, 0x8000); // CS
    write_mem<uint16_t>(VEC_OVERFLOW + 0, 0x0100); // IP

    set_instruction({ 0xce });
    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | CF | OF);
    ASSERT_EQ(read_reg(CS), 0x8000);
    ASSERT_EQ(read_reg(IP), 0x0100);
    ASSERT_EQ(read_reg(SP), 0x0100 - 6);

    ASSERT_EQ(read_mem<uint16_t>(0x100 - 2), FLAGS_STUCK_BITS | CF | IF | TF | OF);
    ASSERT_EQ(read_mem<uint16_t>(0x100 - 4), 0x7c00);
    // Return to the following instruction
    ASSERT_EQ(read_mem<uint16_t>(0x100 - 6), 0x0002);
}
