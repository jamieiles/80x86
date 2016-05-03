#include <gtest/gtest.h>

#include "Emulate.h"
#include "Fifo.h"
#include "Memory.h"
#include "ModRM.h"
#include "RegisterFile.h"

class ModRMEncoder {
public:
    ModRMEncoder()
        : val(0)
    {}

    void reg(Reg r)
    {
        val |= (r << 3);
    }

    void rm_reg(Reg r)
    {
        val |= (r << 0) | (3 << 6);
    }

    void mem_bx()
    {
        val |= (0x7 << 0);
    }

    uint8_t get() const
    {
        return val;
    }
private:
    uint8_t val;
};

uint8_t mod_reg_rm(Reg r1, Reg r2)
{
    ModRMEncoder m;

    m.reg(r1);
    m.rm_reg(r2);

    return m.get();
}

uint8_t mod_reg_rm_mem_bx(Reg r1)
{
    ModRMEncoder m;

    m.reg(r1);
    m.mem_bx();

    return m.get();
}

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

TEST_F(EmulateFixture, MovRegReg)
{
    set_instruction({ 0x88, mod_reg_rm(MODRM_REG_BX, MODRM_REG_AX) });

    registers.set(AX, 0x1);
    registers.set(BX, 0x2);

    auto instr_len = emulator.emulate();
    ASSERT_EQ(2LU, instr_len);

    ASSERT_EQ(0x2, registers.get(AX));
    ASSERT_EQ(0x2, registers.get(BX));
}

TEST_F(EmulateFixture, MovMemReg)
{
    set_instruction({ 0x88, mod_reg_rm_mem_bx(MODRM_REG_AX) });

    registers.set(AX, 0x1234);
    registers.set(BX, 0x0100);
    mem.write<uint16_t>(0x0100, 0);

    auto instr_len = emulator.emulate();
    ASSERT_EQ(2LU, instr_len);

    ASSERT_EQ(0x1234, mem.read<uint16_t>(0x0100));
}
