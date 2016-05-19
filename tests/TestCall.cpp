#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

TEST_F(EmulateFixture, CallDirectIntra)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    write_reg(SP, 0x0100);
    set_instruction({ 0xe8, 0x10, 0x20 });

    emulate();

    ASSERT_EQ(0x00fe, read_reg(SP));
    ASSERT_EQ(0x0033, read_mem<uint16_t>(0x00fe));
    ASSERT_EQ(0x2000, read_reg(CS));
    ASSERT_EQ(0x2010, read_reg(IP));
}

TEST_F(EmulateFixture, CallIndirectIntra)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    write_reg(SP, 0x0100);

    write_reg(BX, 0x0200);
    write_mem<uint16_t>(0x0200, 0x2010);

    // call near [bx]
    set_instruction({ 0xff, 0x17 });

    emulate();

    ASSERT_EQ(0x00fe, read_reg(SP));
    ASSERT_EQ(0x0032, read_mem<uint16_t>(0x00fe));
    ASSERT_EQ(0x2000, read_reg(CS));
    ASSERT_EQ(0x2010, read_reg(IP));
}

TEST_F(EmulateFixture, CallDirectInter)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    write_reg(SP, 0x0100);
    set_instruction({ 0x9a, 0x10, 0x20, 0x00, 0x80 });

    emulate();

    ASSERT_EQ(0x00fc, read_reg(SP));
    ASSERT_EQ(0x2000, read_mem<uint16_t>(0x00fe));
    ASSERT_EQ(0x0035, read_mem<uint16_t>(0x00fc));
    ASSERT_EQ(0x8000, read_reg(CS));
    ASSERT_EQ(0x2010, read_reg(IP));
}

TEST_F(EmulateFixture, CallIndirectInter)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    write_reg(SP, 0x0100);

    write_reg(BX, 0x0200);
    write_mem<uint16_t>(0x0202, 0x8000);
    write_mem<uint16_t>(0x0200, 0x2010);

    // call far [bx]
    set_instruction({ 0xff, 0x1f });

    emulate();

    ASSERT_EQ(0x00fc, read_reg(SP));
    ASSERT_EQ(0x2000, read_mem<uint16_t>(0x00fe));
    ASSERT_EQ(0x0032, read_mem<uint16_t>(0x00fc));
    ASSERT_EQ(0x8000, read_reg(CS));
    ASSERT_EQ(0x2010, read_reg(IP));
}
