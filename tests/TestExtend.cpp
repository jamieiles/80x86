#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

TEST_F(EmulateFixture, CbwPositive)
{
    write_reg(AL, 0x40);
    set_instruction({ 0x98 });

    emulate();

    ASSERT_EQ(read_reg(AX), 0x0040);
    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS);
}

TEST_F(EmulateFixture, CbwNegative)
{
    write_reg(AL, 0x80);
    set_instruction({ 0x98 });

    emulate();

    ASSERT_EQ(read_reg(AX), 0xff80);
    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS);
}
