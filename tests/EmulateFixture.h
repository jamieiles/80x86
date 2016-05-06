#ifndef __EMULATEFIXTURE_H__
#define __EMULATEFIXTURE_H__

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

    void set_instruction(const std::vector<uint8_t> &instr)
    {
        for (auto &b: instr)
            instr_stream.push(b);
    }

    void write_reg(GPR regnum, uint16_t val)
    {
        registers.set(regnum, val);
    }

    uint16_t read_reg(GPR regnum)
    {
        return registers.get(regnum);
    }

    template <typename T>
    void write_mem(uint16_t addr, T val)
    {
        mem.write<T>(addr, val);
    }

    template <typename T>
    T read_mem(uint16_t addr)
    {
        return mem.read<T>(addr);
    }
protected:
    Fifo<uint8_t> instr_stream;
    Memory mem;
    RegisterFile registers;
    Emulator emulator;
};

#endif /* __EMULATEFIXTURE_H__ */
