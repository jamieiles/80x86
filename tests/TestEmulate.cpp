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
