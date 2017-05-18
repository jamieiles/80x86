#include <gtest/gtest.h>

#include "EmulateFixture.h"

TEST_F(EmulateFixture, PushRegFF)
{
    // push ax
    set_instruction({ 0xff, 0xf0 });

    write_reg(AX, 0xaa55);
    write_reg(SP, 0x100);

    emulate();

    ASSERT_EQ(0x0fe, read_reg(SP));
    ASSERT_EQ(0xaa55, read_mem16(0x0fe, SS));
}

TEST_F(EmulateFixture, PushImm16)
{
    // push $0xaa55
    set_instruction({ 0x68, 0x55, 0xaa });

    write_reg(SP, 0x100);

    emulate();

    ASSERT_EQ(0x0fe, read_reg(SP));
    ASSERT_EQ(0xaa55, read_mem16(0x0fe, SS));
}

TEST_F(EmulateFixture, PushImm8)
{
    // push $0x80
    set_instruction({ 0x6a, 0x80 });

    write_reg(SP, 0x100);

    emulate();

    ASSERT_EQ(0x0fe, read_reg(SP));
    ASSERT_EQ(0xff80, read_mem16(0x0fe, SS));
}

TEST_F(EmulateFixture, PushMemFF)
{
    // push [1234]
    set_instruction({ 0xff, 0x36, 0x34, 0x12 });

    write_mem16(0x1234, 0xaa55);
    write_reg(SP, 0x100);

    emulate();

    ASSERT_EQ(0x0fe, read_reg(SP));
    ASSERT_EQ(0xaa55, read_mem16(0x0fe, SS));
}

TEST_F(EmulateFixture, PushReg5X)
{
    // push r
    for (uint8_t i = 0; i < 8; ++i) {
        reset();

        auto reg = static_cast<GPR>(static_cast<int>(AX) + i);
        write_reg(reg, 0x0100 + i);
        write_reg(SP, 0x0100);

        set_instruction({ static_cast<uint8_t>(0x50 + i) });
        emulate();

        ASSERT_EQ(0x0fe, read_reg(SP));
        if (reg != SP)
            ASSERT_EQ(0x0100 + i, read_mem16(0x0fe, SS));
        else
            ASSERT_EQ(0x0100, read_mem16(0x0fe, SS));
    }
}

TEST_F(EmulateFixture, PushSR)
{
    // push sr
    for (uint8_t i = 0; i < 4; ++i) {
        reset();

        auto reg = static_cast<GPR>(static_cast<int>(ES) + i);
        write_reg(reg, 0x0100 + i);
        write_reg(SP, 0x0100);

        set_instruction({ static_cast<uint8_t>((i << 3) | 0x6) });
        emulate();

        ASSERT_EQ(0x0fe, read_reg(SP));
        ASSERT_EQ(0x0100 + i, read_mem16(0x0fe, SS));
    }
}

TEST_F(EmulateFixture, Pusha)
{
    write_reg(AX, 1);
    write_reg(CX, 2);
    write_reg(DX, 3);
    write_reg(BX, 4);
    write_reg(SP, 0x0100);
    write_reg(BP, 5);
    write_reg(SI, 6);
    write_reg(DI, 7);

    set_instruction({ 0x60 });
    emulate();

    EXPECT_EQ(0x0f0, read_reg(SP));
    EXPECT_EQ(1, read_mem16(0xfe, SS));
    EXPECT_EQ(2, read_mem16(0xfc, SS));
    EXPECT_EQ(3, read_mem16(0xfa, SS));
    EXPECT_EQ(4, read_mem16(0xf8, SS));
    EXPECT_EQ(0x100, read_mem16(0xf6, SS));
    EXPECT_EQ(5, read_mem16(0xf4, SS));
    EXPECT_EQ(6, read_mem16(0xf2, SS));
    EXPECT_EQ(7, read_mem16(0xf0, SS));
}

TEST_F(EmulateFixture, Popa)
{
    write_reg(SP, 0xf0);

    write_mem16(0xfe, 1, SS);
    write_mem16(0xfc, 2, SS);
    write_mem16(0xfa, 3, SS);
    write_mem16(0xf8, 4, SS);
    write_mem16(0xf6, 0x800, SS); // Ignored
    write_mem16(0xf4, 5, SS);
    write_mem16(0xf2, 6, SS);
    write_mem16(0xf0, 7, SS);

    set_instruction({ 0x61 });
    emulate();

    EXPECT_EQ(read_reg(AX), 1);
    EXPECT_EQ(read_reg(CX), 2);
    EXPECT_EQ(read_reg(DX), 3);
    EXPECT_EQ(read_reg(BX), 4);
    EXPECT_EQ(read_reg(BP), 5);
    EXPECT_EQ(read_reg(SI), 6);
    EXPECT_EQ(read_reg(DI), 7);
    EXPECT_EQ(read_reg(SP), 0x100);
}

TEST_F(EmulateFixture, PopReg8F)
{
    // pop ax
    set_instruction({ 0x8f, 0xc0 });

    write_mem16(0x0fe, 0xaa55, SS);
    write_reg(SP, 0x0fe);

    emulate();

    ASSERT_EQ(0x100, read_reg(SP));
    ASSERT_EQ(0xaa55, read_reg(AX));
}

TEST_F(EmulateFixture, PopMem8F)
{
    // pop [1234]
    set_instruction({ 0x8f, 0x06, 0x34, 0x12 });

    write_mem16(0x0fe, 0xaa55, SS);
    write_reg(SP, 0x0fe);

    emulate();

    ASSERT_EQ(0x100, read_reg(SP));
    ASSERT_EQ(0xaa55, read_mem16(0x1234, DS));
}

TEST_F(EmulateFixture, PopReg8FInvalidReg)
{
    // pop ax
    set_instruction({ 0x8f, 0xc8 });

    write_mem16(0x0fe, 0xaa55, SS);
    write_reg(SP, 0x0fe);

    emulate();

    ASSERT_EQ(0x0fe, read_reg(SP));
    ASSERT_EQ(0x0, read_reg(AX));

    ASSERT_FALSE(instruction_had_side_effects());
}

TEST_F(EmulateFixture, PopReg5X)
{
    // pop r
    for (uint8_t i = 0; i < 8; ++i) {
        reset();

        auto reg = static_cast<GPR>(static_cast<int>(AX) + i);
        write_mem16(0x0fe, 0x0100 + i, SS);
        write_reg(SP, 0x0fe);

        set_instruction({ static_cast<uint8_t>(0x58 + i) });
        emulate();

        if (reg != SP)
            EXPECT_EQ(0x100, read_reg(SP));
        EXPECT_EQ(0x0100 + i, read_reg(reg));
    }
}

TEST_F(EmulateFixture, PopSR)
{
    // pop sr
    for (uint8_t i = 0; i < 4; ++i) {
        auto reg = static_cast<GPR>(static_cast<int>(ES) + i);
        if (reg == CS)
            continue;

        reset();

        write_reg(SS, 0);

        write_mem16(0x0fe, 0x0100 + i, SS);
        write_reg(SP, 0x0fe);

        set_instruction({ static_cast<uint8_t>((i << 3) | 0x7) });
        emulate();

        ASSERT_EQ(0x100, read_reg(SP));
        ASSERT_EQ(0x0100 + i, read_reg(reg));
    }
}
