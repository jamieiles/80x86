#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

TEST_F(EmulateFixture, ClcClearsCarry)
{
    write_flags(CF);

    set_instruction({ 0xf8 });

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS);
}

TEST_F(EmulateFixture, ClcDoesntSetCarry)
{
    write_flags(0);

    set_instruction({ 0xf8 });

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS);
}

TEST_F(EmulateFixture, CmcInverts0)
{
    write_flags(0);

    set_instruction({ 0xf5 });

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | CF);
}

TEST_F(EmulateFixture, CmcInverts1)
{
    write_flags(CF);

    set_instruction({ 0xf5 });

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS);
}
