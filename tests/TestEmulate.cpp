#include <gtest/gtest.h>

#include "Emulate.h"
#include "Fifo.h"
#include "Memory.h"
#include "ModRM.h"
#include "RegisterFile.h"

class EmulateFixture : public ::testing::Test {
public:
    EmulateFixture()
        : instr_stream(16),
        emulator(&registers)
    {
        emulator.set_instruction_stream(&instr_stream);
        emulator.set_memory(&mem);
    }
    void set_instruction(const std::vector<uint8_t> &instr);
protected:
    Fifo<uint8_t> instr_stream;
    Memory mem;
    RegisterFile registers;
    Emulator emulator;
};

void EmulateFixture::set_instruction(const std::vector<uint8_t> &instr)
{
    for (auto &b: instr)
        instr_stream.push(b);
}

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
