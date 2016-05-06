#include <gtest/gtest.h>

#include "EmulateFixture.h"

TEST_F(EmulateFixture, MovRegReg8)
{
    // mov al, bl
    set_instruction({ 0x88, 0xd8 });

    registers.set(AL, 0x1);
    registers.set(BL, 0x2);

    auto instr_len = emulator.emulate();
    ASSERT_EQ(2LU, instr_len);

    ASSERT_EQ(0x2, registers.get(AL));
    ASSERT_EQ(0x2, registers.get(BL));
}

TEST_F(EmulateFixture, MovMemReg8)
{
    // mov [bx], al
    set_instruction({ 0x88, 0x07 });

    registers.set(AL, 0x12);
    registers.set(BX, 0x100);
    mem.write<uint16_t>(0x0100, 0);

    auto instr_len = emulator.emulate();
    ASSERT_EQ(2LU, instr_len);

    ASSERT_EQ(0x12, mem.read<uint16_t>(0x0100));
}

TEST_F(EmulateFixture, MovRegReg16)
{
    // mov ax, bx
    set_instruction({ 0x89, 0xd8 });

    registers.set(AX, 0x1);
    registers.set(BX, 0x2);

    auto instr_len = emulator.emulate();
    ASSERT_EQ(2LU, instr_len);

    ASSERT_EQ(0x2, registers.get(AL));
    ASSERT_EQ(0x2, registers.get(BL));
}

TEST_F(EmulateFixture, MovMemReg16)
{
    // mov [bx], ax
    set_instruction({ 0x89, 0x07 });

    registers.set(AL, 0x12);
    registers.set(BX, 0x100);
    mem.write<uint16_t>(0x0100, 0);

    auto instr_len = emulator.emulate();
    ASSERT_EQ(2LU, instr_len);

    ASSERT_EQ(0x12, mem.read<uint16_t>(0x0100));
}

TEST_F(EmulateFixture, MovRegReg8_8a)
{
    // mov bl, al
    set_instruction({ 0x8a, 0xd8 });

    registers.set(AL, 0x1);
    registers.set(BL, 0x2);

    auto instr_len = emulator.emulate();
    ASSERT_EQ(2LU, instr_len);

    ASSERT_EQ(0x1, registers.get(AL));
    ASSERT_EQ(0x1, registers.get(BL));
}

TEST_F(EmulateFixture, MovRegMem8)
{
    // mov al, [bx]
    set_instruction({ 0x8a, 0x07 });

    registers.set(AL, 0x12);
    registers.set(BX, 0x100);
    mem.write<uint16_t>(0x0100, 0x55);

    auto instr_len = emulator.emulate();
    ASSERT_EQ(2LU, instr_len);

    ASSERT_EQ(0x55, registers.get(AL));
}

TEST_F(EmulateFixture, MovRegReg16_8b)
{
    // mov bl, al
    set_instruction({ 0x8b, 0xd8 });

    registers.set(AX, 0x1);
    registers.set(BX, 0x2);

    auto instr_len = emulator.emulate();
    ASSERT_EQ(2LU, instr_len);

    ASSERT_EQ(0x1, registers.get(AX));
    ASSERT_EQ(0x1, registers.get(BX));
}

TEST_F(EmulateFixture, MovRegMem16)
{
    // mov ax, [bx]
    set_instruction({ 0x8b, 0x07 });

    registers.set(AL, 0x12);
    registers.set(BX, 0x100);
    mem.write<uint16_t>(0x0100, 0xaa55);

    auto instr_len = emulator.emulate();
    ASSERT_EQ(2LU, instr_len);

    ASSERT_EQ(0xaa55, registers.get(AX));
}

TEST_F(EmulateFixture, MovC6C7RegNot0IsNop)
{
    set_instruction({ 0xc6, 0xff });
    auto instr_len = emulator.emulate();
    ASSERT_EQ(2LU, instr_len);

    set_instruction({ 0xc7, 0xff });
    instr_len = emulator.emulate();
    ASSERT_EQ(2LU, instr_len);
}

TEST_F(EmulateFixture, MovC6RegImmediate)
{
    // mov al, 0xaa
    set_instruction({ 0xc6, 0xc0, 0xaa });
    auto instr_len = emulator.emulate();
    ASSERT_EQ(3LU, instr_len);

    ASSERT_EQ(0xaa, registers.get(AL));
}

TEST_F(EmulateFixture, MovC6MemImmediate)
{
    // mov [bx], 0xaa
    set_instruction({ 0xc6, 0x07, 0xaa });
    registers.set(BX, 0x100);
    auto instr_len = emulator.emulate();
    ASSERT_EQ(3LU, instr_len);

    ASSERT_EQ(0xaa, mem.read<uint8_t>(0x100));
}

TEST_F(EmulateFixture, MovC7RegImmediate)
{
    // mov ax, 0xaa55
    set_instruction({ 0xc7, 0xc0, 0x55, 0xaa });
    auto instr_len = emulator.emulate();
    ASSERT_EQ(4LU, instr_len);

    ASSERT_EQ(0xaa55, registers.get(AX));
}

TEST_F(EmulateFixture, MovC7MemImmediate)
{
    // mov [bx], 0xaa55
    set_instruction({ 0xc7, 0x07, 0x55, 0xaa });
    registers.set(BX, 0x100);
    auto instr_len = emulator.emulate();
    ASSERT_EQ(4LU, instr_len);

    ASSERT_EQ(0xaa55, mem.read<uint16_t>(0x100));
}

TEST_F(EmulateFixture, MovRegImmediate8)
{
    // mov al, 0xaa
    for (uint8_t i = 0; i < 8; ++i) {
        auto reg = static_cast<GPR>(static_cast<int>(AL) + i);
        registers.set(reg, 0);

        set_instruction({ static_cast<uint8_t>(0xb0 + i), 0xaa });
        auto instr_len = emulator.emulate();
        ASSERT_EQ(2LU, instr_len);

        ASSERT_EQ(0xaa, registers.get(reg));
    }
}

TEST_F(EmulateFixture, MovRegImmediate16)
{
    // mov al, 0xaa55
    for (uint8_t i = 0; i < 8; ++i) {
        auto reg = static_cast<GPR>(static_cast<int>(AX) + i);
        registers.set(reg, 0);

        set_instruction({ static_cast<uint8_t>(0xb8 + i), 0x55, 0xaa });
        auto instr_len = emulator.emulate();
        ASSERT_EQ(3LU, instr_len);

        ASSERT_EQ(0xaa55, registers.get(reg));
    }
}

TEST_F(EmulateFixture, MovAccumulatorMemory8)
{
    // mov al, [1234]
    set_instruction({ 0xa0, 0x34, 0x12 });
    mem.write<uint16_t>(0x1234, 0xaa55);
    auto instr_len = emulator.emulate();
    ASSERT_EQ(3LU, instr_len);

    ASSERT_EQ(0x55, registers.get(AL));
}

TEST_F(EmulateFixture, MovAccumulatorMemory16)
{
    // mov ax, [1234]
    set_instruction({ 0xa1, 0x34, 0x12 });
    mem.write<uint16_t>(0x1234, 0xaa55);
    auto instr_len = emulator.emulate();
    ASSERT_EQ(3LU, instr_len);

    ASSERT_EQ(0xaa55, registers.get(AX));
}

TEST_F(EmulateFixture, MovMemoryAccumulator8)
{
    // mov [1234], al
    set_instruction({ 0xa2, 0x34, 0x12 });
    registers.set(AL, 0x55);
    auto instr_len = emulator.emulate();
    ASSERT_EQ(3LU, instr_len);

    ASSERT_EQ(0x55, mem.read<uint8_t>(0x1234));
}

TEST_F(EmulateFixture, MovMemoryAccumulator16)
{
    // mov [1234], ax
    set_instruction({ 0xa3, 0x34, 0x12 });
    registers.set(AX, 0xaa55);
    auto instr_len = emulator.emulate();
    ASSERT_EQ(3LU, instr_len);

    ASSERT_EQ(0xaa55, mem.read<uint16_t>(0x1234));
}

TEST_F(EmulateFixture, MovSRReg)
{
    // mov cs, ax
    set_instruction({ 0x8e, 0xc8 });
    registers.set(AX, 0x8000);
    auto instr_len = emulator.emulate();
    ASSERT_EQ(2LU, instr_len);

    ASSERT_EQ(0x8000, registers.get(CS));
}

TEST_F(EmulateFixture, MovSRMem)
{
    // mov cs, [bx]
    set_instruction({ 0x8e, 0x0f });
    mem.write<uint16_t>(0x0100, 0x8000);
    registers.set(BX, 0x0100);
    auto instr_len = emulator.emulate();
    ASSERT_EQ(2LU, instr_len);

    ASSERT_EQ(0x8000, registers.get(CS));
}

TEST_F(EmulateFixture, MovRegSR)
{
    // mov ax, cs
    set_instruction({ 0x8c, 0xc8 });
    registers.set(CS, 0x8000);
    auto instr_len = emulator.emulate();
    ASSERT_EQ(2LU, instr_len);

    ASSERT_EQ(0x8000, registers.get(AX));
}

TEST_F(EmulateFixture, MovMemSR)
{
    // mov [bx], cs
    set_instruction({ 0x8c, 0x0f });
    registers.set(BX, 0x0100);
    registers.set(CS, 0x8000);
    auto instr_len = emulator.emulate();
    ASSERT_EQ(2LU, instr_len);

    ASSERT_EQ(0x8000, mem.read<uint16_t>(0x0100));
}
