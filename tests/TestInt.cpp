#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

// flags
// cs
// ip
TEST_F(EmulateFixture, Int3)
{
    // int 3
    set_instruction({ 0xcc });
    write_reg(SP, 0x100);
    write_reg(CS, 0x7c00);
    write_reg(IP, 0x0001);
    write_flags(CF | IF | TF);

    write_mem<uint16_t>(12 + 2, 0x8000); // CS
    write_mem<uint16_t>(12 + 0, 0x0100); // IP

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | CF);
    ASSERT_EQ(read_reg(CS), 0x8000);
    ASSERT_EQ(read_reg(IP), 0x0100);
    ASSERT_EQ(read_reg(SP), 0x0100 - 6);

    ASSERT_EQ(read_mem<uint16_t>(0x100 - 2), FLAGS_STUCK_BITS | CF | IF | TF);
    ASSERT_EQ(read_mem<uint16_t>(0x100 - 4), 0x7c00);
    ASSERT_EQ(read_mem<uint16_t>(0x100 - 6), 0x0001);
}
