#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

TEST_F(EmulateFixture, JmpDirectIntra)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    set_instruction({ 0xe9, 0x10, 0x20 });

    emulate();

    ASSERT_EQ(0x2000, read_reg(CS));
    ASSERT_EQ(0x2043, read_reg(IP));
}

TEST_F(EmulateFixture, JmpDirectIntraShort)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0100);
    set_instruction({ 0xeb, 0x80 });

    emulate();

    ASSERT_EQ(0x2000, read_reg(CS));
    ASSERT_EQ(0x0082, read_reg(IP));
}

TEST_F(EmulateFixture, JmpIndirectIntraReg)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    write_reg(AX, 0x0100);
    // jmp ax
    set_instruction({ 0xff, 0xe0 });

    emulate();

    ASSERT_EQ(0x2000, read_reg(CS));
    ASSERT_EQ(0x0100, read_reg(IP));
}

TEST_F(EmulateFixture, JmpIndirectIntraMem)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);

    write_reg(BX, 0x0800);
    write_mem<uint16_t>(0x0800, 0x0100);
    // jmp [bx]
    set_instruction({ 0xff, 0x27 });

    emulate();

    ASSERT_EQ(0x2000, read_reg(CS));
    ASSERT_EQ(0x0100, read_reg(IP));
}

TEST_F(EmulateFixture, DirectInter)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);

    // jmp 0x8000:0x1234
    set_instruction({ 0xea, 0x34, 0x12, 0x00, 0x80 });

    emulate();

    ASSERT_EQ(0x8000, read_reg(CS));
    ASSERT_EQ(0x1234, read_reg(IP));
}

TEST_F(EmulateFixture, JmpIndirectInterReg)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    // jmp far ax (invalid encoding)
    set_instruction({ 0xff, 0xe8 });

    emulate();

    ASSERT_EQ(0x2000, read_reg(CS));
    ASSERT_EQ(0x0032, read_reg(IP));
}

TEST_F(EmulateFixture, JmpIndirectInterMem)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);

    write_reg(BX, 0x0800);
    write_mem<uint16_t>(0x0800, 0x5678);
    write_mem<uint16_t>(0x0802, 0x4000);
    // jmp far [bx]
    set_instruction({ 0xff, 0x2f });

    emulate();

    ASSERT_EQ(0x4000, read_reg(CS));
    ASSERT_EQ(0x5678, read_reg(IP));
}
