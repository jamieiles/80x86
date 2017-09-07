#include <gtest/gtest.h>

#include "EmulateFixture.h"

TEST_F(EmulateFixture, XchgRegReg8)
{
    // xchg al, bl
    set_instruction({0x86, 0xc3});
    write_reg(AL, 0xaa);
    write_reg(BL, 0xbb);

    emulate();

    ASSERT_EQ(read_reg(AL), 0xbb);
    ASSERT_EQ(read_reg(BL), 0xaa);
}

TEST_F(EmulateFixture, XchgRegReg16)
{
    // xchg bx, cx
    set_instruction({0x87, 0xd9});
    write_reg(BX, 0xbbbb);
    write_reg(CX, 0xcccc);

    emulate();

    ASSERT_EQ(read_reg(BX), 0xcccc);
    ASSERT_EQ(read_reg(CX), 0xbbbb);
}

TEST_F(EmulateFixture, XchgRegMem8)
{
    // xchg al, [0x1234]
    set_instruction({0x86, 0x06, 0x34, 0x12});
    write_mem8(0x1234, 0x12);
    write_reg(AL, 0xaa);

    emulate();

    ASSERT_EQ(read_reg(AL), 0x12);
    ASSERT_EQ(read_mem8(0x1234), 0xaa);
}

TEST_F(EmulateFixture, XchgRegMem16)
{
    // xchg ax, [0x1234]
    set_instruction({0x87, 0x06, 0x34, 0x12});
    write_mem16(0x1234, 0x1234);
    write_reg(AX, 0xaaaa);

    emulate();

    ASSERT_EQ(read_reg(AX), 0x1234);
    ASSERT_EQ(read_mem16(0x1234), 0xaaaa);
}

TEST_F(EmulateFixture, XchgALALNop)
{
    // xchg al, al
    set_instruction({0x86, 0xc0});
    write_reg(AL, 0xaa);

    emulate();

    ASSERT_EQ(read_reg(AL), 0xaa);
}

class XchgAXFixture
    : public EmulateFixture,
      public ::testing::WithParamInterface<std::pair<uint8_t, GPR>>
{
};
TEST_P(XchgAXFixture, XchgRegAccumulator)
{
    // xchg ax, ?x
    set_instruction({GetParam().first});
    write_reg(AX, 0xaaaa);
    write_reg(GetParam().second, 0xbbbb);

    emulate();

    ASSERT_EQ(read_reg(AX), 0xbbbb);
    ASSERT_EQ(read_reg(GetParam().second), 0xaaaa);
}
INSTANTIATE_TEST_CASE_P(
    XchgAccumulator,
    XchgAXFixture,
    ::testing::Values(std::make_pair<uint8_t, GPR>(0x91, CX),
                      std::make_pair<uint8_t, GPR>(0x92, DX),
                      std::make_pair<uint8_t, GPR>(0x93, BX),
                      std::make_pair<uint8_t, GPR>(0x94, SP),
                      std::make_pair<uint8_t, GPR>(0x95, BP),
                      std::make_pair<uint8_t, GPR>(0x96, SI),
                      std::make_pair<uint8_t, GPR>(0x97, DI)));
