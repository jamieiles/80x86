#ifndef __EMULATEFIXTURE_H__
#define __EMULATEFIXTURE_H__

#include <gtest/gtest.h>

#include <cassert>
#include "CPU.h"

class EmulateFixture : public ::testing::Test {
public:
    EmulateFixture()
    {
        // Doesn't need to model the reset vector of FFF0:0000 otherwise we
        // need to handle wrapping around to 0.  Just start off away from the
        // interrupt vector table so we can easily detect interrupts.
        cpu.write_reg(IP, 0x1000);
    }

    void set_instruction(const std::vector<uint8_t> &instr)
    {
        for (size_t m = 0; m < instr.size(); ++m)
            cpu.write_mem<uint8_t>(get_phys_addr(cpu.read_reg(CS), cpu.read_reg(IP) + m),
                                   instr[m]);
        instr_len = instr.size();
    }

    void write_reg(GPR regnum, uint16_t val)
    {
        cpu.write_reg(regnum, val);
    }

    uint16_t read_reg(GPR regnum)
    {
        return cpu.read_reg(regnum);
    }

    template <typename T>
    void write_mem(uint32_t addr, T val)
    {
        cpu.write_mem<T>(addr, val);
    }

    void write_cstring(uint32_t addr, const char *str)
    {
        do {
            write_mem<uint8_t>(addr++, *str++);
        } while (*str);
    }

    template <typename T>
    T read_mem(uint32_t addr)
    {
        return cpu.read_mem<T>(addr);
    }

    template <typename T>
    void write_io(uint32_t addr, T val)
    {
        cpu.write_io<T>(addr, val);
    }

    template <typename T>
    T read_io(uint32_t addr)
    {
        return cpu.read_io<T>(addr);
    }

    void emulate(int count=1)
    {
        assert(count > 0);
        size_t len = 0;
        for (auto i = 0; i < count; ++i)
             len += cpu.cycle();
        ASSERT_EQ(len, instr_len);
    }

    void write_flags(uint16_t val)
    {
        cpu.write_flags(val);
    }

    uint16_t read_flags()
    {
        return cpu.read_flags();
    }
protected:
    size_t instr_len;
    CPU cpu;
};

#endif /* __EMULATEFIXTURE_H__ */
