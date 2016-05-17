#ifndef __EMULATEFIXTURE_H__
#define __EMULATEFIXTURE_H__

#include <gtest/gtest.h>

#include <cassert>
#include "Emulate.h"
#include "Memory.h"
#include "ModRM.h"
#include "RegisterFile.h"

class EmulateFixture : public ::testing::Test {
public:
    EmulateFixture()
        : emulator(&registers)
    {
        emulator.set_memory(&mem);
        emulator.set_io(&io);
    }

    void set_instruction(const std::vector<uint8_t> &instr)
    {
        for (size_t m = 0; m < instr.size(); ++m)
            mem.write<uint8_t>(get_phys_addr(registers.get(CS), registers.get(IP) + m),
                               instr[m]);
        instr_len = instr.size();
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

    template <typename T>
    void write_io(uint16_t addr, T val)
    {
        io.write<T>(addr, val);
    }

    template <typename T>
    T read_io(uint16_t addr)
    {
        return io.read<T>(addr);
    }

    void emulate(int count=1)
    {
        assert(count > 0);
        size_t len = 0;
        for (auto i = 0; i < count; ++i)
             len += emulator.emulate();
        ASSERT_EQ(len, instr_len);
    }

    void write_flags(uint16_t val)
    {
        registers.set_flags(val);
    }

    uint16_t read_flags()
    {
        return registers.get_flags();
    }
protected:
    size_t instr_len;
    Memory mem;
    Memory io;
    RegisterFile registers;
    Emulator emulator;
};

#endif /* __EMULATEFIXTURE_H__ */
