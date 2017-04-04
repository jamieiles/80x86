#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

// flags
// cs
// ip
TEST_F(EmulateFixture, Int3)
{
    write_mem16(VEC_INT + 2, 0x8000, CS); // CS
    write_mem16(VEC_INT + 0, 0x0100, CS); // IP

    // int 3
    write_reg(SP, 0x100);
    write_reg(CS, 0x7c00);
    write_reg(IP, 0x0001);
    write_flags(CF | IF | TF);

    set_instruction({ 0xcc });
    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | CF);
    ASSERT_EQ(read_reg(CS), 0x8000);
    ASSERT_EQ(read_reg(IP), 0x0100);
    ASSERT_EQ(read_reg(SP), 0x0100 - 6);

    ASSERT_EQ(read_mem16(0x100 - 2, SS), FLAGS_STUCK_BITS | CF | IF | TF);
    ASSERT_EQ(read_mem16(0x100 - 4, SS), 0x7c00);
    // Return to the following instruction
    ASSERT_EQ(read_mem16(0x100 - 6, SS), 0x0002);
}

TEST_F(EmulateFixture, IntN)
{
    write_mem16(VEC_INT + 2, 0x8000, CS); // CS
    write_mem16(VEC_INT + 0, 0x0100, CS); // IP

    // int 3
    write_reg(SP, 0x100);
    write_reg(CS, 0x7c00);
    write_reg(IP, 0x0001);
    write_flags(CF | IF | TF);

    set_instruction({ 0xcd, 0x03 });
    emulate();

    EXPECT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | CF);
    EXPECT_EQ(read_reg(CS), 0x8000);
    EXPECT_EQ(read_reg(IP), 0x0100);
    EXPECT_EQ(read_reg(SP), 0x0100 - 6);

    EXPECT_EQ(read_mem16(0x100 - 2, SS), FLAGS_STUCK_BITS | CF | IF | TF);
    EXPECT_EQ(read_mem16(0x100 - 4, SS), 0x7c00);
    // Return to the following instruction
    EXPECT_EQ(read_mem16(0x100 - 6, SS), 0x0003);
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
    write_mem16(VEC_OVERFLOW + 2, 0x8000, CS); // CS
    write_mem16(VEC_OVERFLOW + 0, 0x0100, CS); // IP

    // int 3
    write_reg(SP, 0x100);
    write_reg(CS, 0x7c00);
    write_reg(IP, 0x0001);
    write_flags(CF | IF | TF | OF);

    set_instruction({ 0xce });
    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | CF | OF);
    ASSERT_EQ(read_reg(CS), 0x8000);
    ASSERT_EQ(read_reg(IP), 0x0100);
    ASSERT_EQ(read_reg(SP), 0x0100 - 6);

    ASSERT_EQ(read_mem16(0x100 - 2, SS), FLAGS_STUCK_BITS | CF | IF | TF | OF);
    ASSERT_EQ(read_mem16(0x100 - 4, SS), 0x7c00);
    // Return to the following instruction
    ASSERT_EQ(read_mem16(0x100 - 6, SS), 0x0002);
}
