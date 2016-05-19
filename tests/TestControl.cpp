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

TEST_F(EmulateFixture, StcSetsCarry)
{
    write_flags(0);

    set_instruction({ 0xf9 });

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | CF);
}

TEST_F(EmulateFixture, StcDoesntClearCarry)
{
    write_flags(CF);

    set_instruction({ 0xf9 });

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | CF);
}

TEST_F(EmulateFixture, CldClearsDirection)
{
    write_flags(DF);

    set_instruction({ 0xfc });

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS);
}

TEST_F(EmulateFixture, CldDoesntSetDirection)
{
    write_flags(0);

    set_instruction({ 0xfc });

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS);
}

TEST_F(EmulateFixture, StdSetsDirection)
{
    write_flags(0);

    set_instruction({ 0xfd });

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | DF);
}

TEST_F(EmulateFixture, StdDoesntClearDirection)
{
    write_flags(DF);

    set_instruction({ 0xfd });

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | DF);
}

TEST_F(EmulateFixture, CliClearsInterrupt)
{
    write_flags(IF);

    set_instruction({ 0xfa });

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS);
}

TEST_F(EmulateFixture, CliDoesntSetInterrupt)
{
    write_flags(0);

    set_instruction({ 0xfa });

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS);
}

TEST_F(EmulateFixture, StiSetsInterrupt)
{
    write_flags(0);

    set_instruction({ 0xfb });

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | IF);
}

TEST_F(EmulateFixture, StiDoesntClearInterrupt)
{
    write_flags(IF);

    set_instruction({ 0xfb });

    emulate();

    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS | IF);
}
