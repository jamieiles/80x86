#include <gtest/gtest.h>

#include "EmulateFixture.h"

TEST_F(EmulateFixture, XchgRegReg8)
{
    // xchg al, bl
    set_instruction({ 0x86, 0xc3 });
    write_reg(AL, 0xaa);
    write_reg(BL, 0xbb);

    emulate();

    ASSERT_EQ(read_reg(AL), 0xbb);
    ASSERT_EQ(read_reg(BL), 0xaa);
}

TEST_F(EmulateFixture, XchgRegReg16)
{
    // xchg bx, cx
    set_instruction({ 0x87, 0xd9 });
    write_reg(BX, 0xbbbb);
    write_reg(CX, 0xcccc);

    emulate();

    ASSERT_EQ(read_reg(BX), 0xcccc);
    ASSERT_EQ(read_reg(CX), 0xbbbb);
}

TEST_F(EmulateFixture, XchgRegMem8)
{
    // xchg al, [0x1234]
    set_instruction({ 0x86, 0x06, 0x34, 0x12 });
    write_mem<uint8_t>(0x1234, 0x12);
    write_reg(AL, 0xaa);

    emulate();

    ASSERT_EQ(read_reg(AL), 0x12);
    ASSERT_EQ(read_mem<uint8_t>(0x1234), 0xaa);
}

TEST_F(EmulateFixture, XchgRegMem16)
{
    // xchg ax, [0x1234]
    set_instruction({ 0x87, 0x06, 0x34, 0x12 });
    write_mem<uint16_t>(0x1234, 0x1234);
    write_reg(AX, 0xaaaa);

    emulate();

    ASSERT_EQ(read_reg(AX), 0x1234);
    ASSERT_EQ(read_mem<uint16_t>(0x1234), 0xaaaa);
}

TEST_F(EmulateFixture, XchgALALNop)
{
    // xchg al, al
    set_instruction({ 0x86, 0xc0 });
    write_reg(AL, 0xaa);

    emulate();

    ASSERT_EQ(read_reg(AL), 0xaa);
}

TEST_F(EmulateFixture, XchgRegAccumulator)
{
    // xchg ax, bx
    set_instruction({ 0x93 });
    write_reg(AX, 0xaaaa);
    write_reg(BX, 0xbbbb);

    emulate();

    ASSERT_EQ(read_reg(AX), 0xbbbb);
    ASSERT_EQ(read_reg(BX), 0xaaaa);
}
