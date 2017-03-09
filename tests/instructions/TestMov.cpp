#include <gtest/gtest.h>

#include "EmulateFixture.h"

TEST_F(EmulateFixture, MovRegReg8)
{
    // mov al, bl
    set_instruction({ 0x88, 0xd8 });

    write_reg(AL, 0x1);
    write_reg(BL, 0x2);

    emulate();

    ASSERT_EQ(0x2, read_reg(AL));
    ASSERT_EQ(0x2, read_reg(BL));
}

TEST_F(EmulateFixture, MovMemReg8)
{
    // mov [bx], al
    set_instruction({ 0x88, 0x07 });

    write_reg(AL, 0x12);
    write_reg(BX, 0x100);
    write_mem<uint16_t>(0x0100, 0);

    emulate();

    ASSERT_EQ(0x12, read_mem<uint16_t>(0x0100));
}

TEST_F(EmulateFixture, MovRegReg16)
{
    // mov ax, bx
    set_instruction({ 0x89, 0xd8 });

    write_reg(AX, 0x1);
    write_reg(BX, 0x2);

    emulate();

    ASSERT_EQ(0x2, read_reg(AL));
    ASSERT_EQ(0x2, read_reg(BL));
}

TEST_F(EmulateFixture, MovMemReg16)
{
    // mov [bx], ax
    set_instruction({ 0x89, 0x07 });

    write_reg(AL, 0x12);
    write_reg(BX, 0x100);
    write_mem<uint16_t>(0x0100, 0);

    emulate();

    ASSERT_EQ(0x12, read_mem<uint16_t>(0x0100));
}

TEST_F(EmulateFixture, MovRegReg8_8a)
{
    // mov bl, al
    set_instruction({ 0x8a, 0xd8 });

    write_reg(AL, 0x1);
    write_reg(BL, 0x2);

    emulate();

    ASSERT_EQ(0x1, read_reg(AL));
    ASSERT_EQ(0x1, read_reg(BL));
}

TEST_F(EmulateFixture, MovRegMem8)
{
    // mov al, [bx]
    set_instruction({ 0x8a, 0x07 });

    write_reg(AL, 0x12);
    write_reg(BX, 0x100);
    write_mem<uint16_t>(0x0100, 0x55);

    emulate();

    ASSERT_EQ(0x55, read_reg(AL));
}

TEST_F(EmulateFixture, MovRegReg16_8b)
{
    // mov bl, al
    set_instruction({ 0x8b, 0xd8 });

    write_reg(AX, 0x1);
    write_reg(BX, 0x2);

    emulate();

    ASSERT_EQ(0x1, read_reg(AX));
    ASSERT_EQ(0x1, read_reg(BX));
}

TEST_F(EmulateFixture, MovRegMem16)
{
    // mov ax, [bx]
    set_instruction({ 0x8b, 0x07 });

    write_reg(AL, 0x12);
    write_reg(BX, 0x100);
    write_mem<uint16_t>(0x0100, 0xaa55);

    emulate();

    ASSERT_EQ(0xaa55, read_reg(AX));
}

TEST_F(EmulateFixture, MovC6C7RegNot0IsNop)
{
    set_instruction({ 0xc6, 0xff });
    emulate();
    ASSERT_EQ(2LU, instr_len);

    reset();

    set_instruction({ 0xc7, 0xff });
    emulate();
    ASSERT_EQ(2LU, instr_len);
}

TEST_F(EmulateFixture, MovC6RegImmediate)
{
    // mov al, 0xaa
    set_instruction({ 0xc6, 0xc0, 0xaa });
    emulate();

    ASSERT_EQ(0xaa, read_reg(AL));
}

TEST_F(EmulateFixture, MovC6MemImmediate)
{
    // mov [bx], 0xaa
    set_instruction({ 0xc6, 0x07, 0xaa });
    write_reg(BX, 0x100);
    emulate();

    ASSERT_EQ(0xaa, read_mem<uint8_t>(0x100));
}

TEST_F(EmulateFixture, MovC7RegImmediate)
{
    // mov ax, 0xaa55
    set_instruction({ 0xc7, 0xc0, 0x55, 0xaa });
    emulate();

    ASSERT_EQ(0xaa55, read_reg(AX));
}

TEST_F(EmulateFixture, MovC7MemImmediate)
{
    // mov [bx], 0xaa55
    set_instruction({ 0xc7, 0x07, 0x55, 0xaa });
    write_reg(BX, 0x100);
    emulate();

    ASSERT_EQ(0xaa55, read_mem<uint16_t>(0x100));
}

TEST_F(EmulateFixture, MovRegImmediate8)
{
    // mov al, 0xaa
    for (uint8_t i = 0; i < 8; ++i) {
        auto reg = static_cast<GPR>(static_cast<int>(AL) + i);
        write_reg(reg, 0);

        set_instruction({ static_cast<uint8_t>(0xb0 + i), 0xaa });
        emulate();

        ASSERT_EQ(0xaa, read_reg(reg));
    }
}

TEST_F(EmulateFixture, MovRegImmediate16)
{
    // mov al, 0xaa55
    for (uint8_t i = 0; i < 8; ++i) {
        auto reg = static_cast<GPR>(static_cast<int>(AX) + i);
        write_reg(reg, 0);

        set_instruction({ static_cast<uint8_t>(0xb8 + i), 0x55, 0xaa });
        emulate();

        ASSERT_EQ(0xaa55, read_reg(reg));
    }
}

TEST_F(EmulateFixture, MovAccumulatorMemory8)
{
    // mov al, [1234]
    set_instruction({ 0xa0, 0x34, 0x12 });
    write_mem<uint16_t>(0x1234, 0xaa55);
    emulate();

    ASSERT_EQ(0x55, read_reg(AL));
}

TEST_F(EmulateFixture, MovAccumulatorMemory16)
{
    // mov ax, [1234]
    set_instruction({ 0xa1, 0x34, 0x12 });
    write_mem<uint16_t>(0x1234, 0xaa55);
    emulate();

    ASSERT_EQ(0xaa55, read_reg(AX));
}

TEST_F(EmulateFixture, MovMemoryAccumulator8)
{
    // mov [1234], al
    set_instruction({ 0xa2, 0x34, 0x12 });
    write_reg(AL, 0x55);
    emulate();

    ASSERT_EQ(0x55, read_mem<uint8_t>(0x1234));
}

TEST_F(EmulateFixture, MovMemoryAccumulator16)
{
    // mov [1234], ax
    set_instruction({ 0xa3, 0x34, 0x12 });
    write_reg(AX, 0xaa55);
    emulate();

    ASSERT_EQ(0xaa55, read_mem<uint16_t>(0x1234));
}

TEST_F(EmulateFixture, MovSRReg)
{
    // mov es, ax
    set_instruction({ 0x8e, 0xc0 });
    write_reg(AX, 0x8000);
    emulate();

    ASSERT_EQ(0x8000, read_reg(ES));
}

TEST_F(EmulateFixture, MovSRMem)
{
    // mov es, [bx]
    set_instruction({ 0x8e, 0x07 });
    write_mem<uint16_t>(0x0100, 0x8000);
    write_reg(BX, 0x0100);
    emulate();

    ASSERT_EQ(0x8000, read_reg(ES));
}

TEST_F(EmulateFixture, MovSRInvalidSR)
{
    set_instruction({ 0x8e, 0xe8 });
    write_reg(CS, 0);
    write_reg(AX, 0x8000);
    emulate();
    ASSERT_EQ(0, read_reg(CS));

    ASSERT_FALSE(instruction_had_side_effects());
}

TEST_F(EmulateFixture, MovRegSR)
{
    // mov ax, cs
    write_reg(CS, 0x8000);
    set_instruction({ 0x8c, 0xc8 });
    emulate();

    ASSERT_EQ(0x8000, read_reg(AX));
}

TEST_F(EmulateFixture, MovMemSR)
{
    // mov [bx], cs
    write_reg(BX, 0x0100);
    write_reg(CS, 0x8000);
    set_instruction({ 0x8c, 0x0f });
    emulate();

    ASSERT_EQ(0x8000, read_mem<uint16_t>(0x0100));
}

TEST_F(EmulateFixture, MovRegInvalidSR)
{
    // mov ax, cs
    write_reg(AX, 0);
    write_reg(CS, 0x8000);
    set_instruction({ 0x8c, 0xe8 });
    emulate();

    ASSERT_EQ(0, read_reg(AX));

    ASSERT_FALSE(instruction_had_side_effects());
}
