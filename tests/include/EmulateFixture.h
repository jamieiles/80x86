#pragma once

#include <memory>
#include <vector>
#include <gtest/gtest.h>

#include <cassert>
#include "CPU.h"
#include "TestUtils.h"

extern std::unique_ptr<CPU> get_cpu(const std::string &test_name);

class EmulateFixture : public ::testing::Test {
public:
    EmulateFixture()
        : cpu(get_cpu(current_test_name()))
    {
        reset();
    }

    void reset()
    {
        cpu->reset();
        // Doesn't need to model the reset vector of FFF0:0000 otherwise we
        // need to handle wrapping around to 0.  Just start off away from the
        // interrupt vector table so we can easily detect interrupts.
        cpu->write_reg(IP, 0x1000);
        cpu->write_reg(SS, 0x2000);
        cpu->write_reg(ES, 0x4000);
        cpu->write_reg(DS, 0x6000);
    }

    void set_instruction(const std::vector<uint8_t> &instr)
    {
        for (size_t m = 0; m < instr.size(); ++m)
            cpu->write_mem<uint8_t>(get_phys_addr(cpu->read_reg(CS), cpu->read_reg(IP) + m),
                                    instr[m]);
        instr_len = instr.size();
    }

    void write_reg(GPR regnum, uint16_t val)
    {
        cpu->write_reg(regnum, val);
    }

    uint16_t read_reg(GPR regnum)
    {
        return cpu->read_reg(regnum);
    }

    template <typename T>
    void write_mem(uint32_t addr, T val, GPR segment=DS)
    {
        cpu->write_mem<T>(get_phys_addr(cpu->read_reg(segment), addr), val);
    }

    void write_cstring(uint32_t addr, const char *str, GPR segment=DS)
    {
        do {
            write_mem<uint8_t>(addr++, *str++, segment);
        } while (*str);
        write_mem<uint8_t>(addr, 0, segment);
    }

    template <typename T>
    void write_vector(uint32_t addr, std::vector<T> vec, GPR segment=DS)
    {
        for (auto v: vec) {
            write_mem<T>(addr, v, segment);
            addr += sizeof(T);
        }
    }

    std::string read_cstring(uint32_t addr, GPR segment=DS)
    {
        std::string str;

        char v;
        for (;;) {
            v = read_mem<uint8_t>(addr++, segment);
            if (!v)
                break;
            str += v;
        }

        return str;
    }

    template <typename T>
    T read_mem(uint32_t addr, GPR segment=DS)
    {
        return cpu->read_mem<T>(get_phys_addr(cpu->read_reg(segment), addr));
    }

    template <typename T>
    void write_io(uint32_t addr, T val)
    {
        cpu->write_io<T>(addr, val);
    }

    template <typename T>
    T read_io(uint32_t addr)
    {
        return cpu->read_io<T>(addr);
    }

    void emulate(int count=1)
    {
        cpu->clear_side_effects();

        assert(count > 0);
        size_t len = 0;
        for (auto i = 0; i < count; ++i)
             len += cpu->step();
        ASSERT_EQ(len, instr_len);
    }

    void write_flags(uint16_t val)
    {
        cpu->write_flags(val);
    }

    uint16_t read_flags()
    {
        return cpu->read_flags();
    }

    bool instruction_had_side_effects()
    {
        return cpu->instruction_had_side_effects();
    }
protected:
    size_t instr_len;
    std::unique_ptr<CPU> cpu;
};
