#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

TEST_F(EmulateFixture, RetIntra)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    write_reg(SP, 0x00fe);
    write_mem16(0x00fe, 0x0100, SS);

    set_instruction({0xc3});

    emulate();

    ASSERT_EQ(0x0100, read_reg(SP));
    ASSERT_EQ(0x2000, read_reg(CS));
    ASSERT_EQ(0x0100, read_reg(IP));
}

TEST_F(EmulateFixture, RetIntraAddSp)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    write_reg(SP, 0x00fe);
    write_mem16(0x00fe, 0x0100, SS);

    set_instruction({0xc2, 0x10, 0x00});

    emulate();

    ASSERT_EQ(0x0110, read_reg(SP));
    ASSERT_EQ(0x2000, read_reg(CS));
    ASSERT_EQ(0x0100, read_reg(IP));
}

TEST_F(EmulateFixture, RetInter)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    write_reg(SP, 0x00fc);
    write_mem16(0x00fe, 0x8000, SS);
    write_mem16(0x00fc, 0x0100, SS);

    set_instruction({0xcb});

    emulate();

    ASSERT_EQ(0x0100, read_reg(SP));
    ASSERT_EQ(0x8000, read_reg(CS));
    ASSERT_EQ(0x0100, read_reg(IP));
}

TEST_F(EmulateFixture, RetInterAddSp)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    write_reg(SP, 0x00fc);
    write_mem16(0x00fe, 0x8000, SS);
    write_mem16(0x00fc, 0x0100, SS);

    set_instruction({0xca, 0x10, 0x00});

    emulate();

    ASSERT_EQ(0x0110, read_reg(SP));
    ASSERT_EQ(0x8000, read_reg(CS));
    ASSERT_EQ(0x0100, read_reg(IP));
}

TEST_F(EmulateFixture, Iret)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    write_reg(SP, 0x00fa);
    write_mem16(0x00fe, FLAGS_STUCK_BITS | CF, SS);
    write_mem16(0x00fc, 0x8000, SS);
    write_mem16(0x00fa, 0x0100, SS);

    set_instruction({0xcf});

    emulate();

    ASSERT_EQ(0x0100, read_reg(SP));
    ASSERT_EQ(0x8000, read_reg(CS));
    ASSERT_EQ(0x0100, read_reg(IP));
    ASSERT_PRED_FORMAT2(AssertFlagsEqual, read_flags(), FLAGS_STUCK_BITS | CF);
}
