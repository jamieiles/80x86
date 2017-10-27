// Copyright Jamie Iles, 2017
//
// This file is part of s80x86.
//
// s80x86 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// s80x86 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with s80x86.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <cassert>
#include <string>

#include "CPU.h"
#include "RegisterFile.h"

class JTAGCPU : public CPU
{
public:
    JTAGCPU(const std::string &test_name);
    ~JTAGCPU();
    void reset();
    void write_reg(GPR regnum, uint16_t val);
    uint16_t read_reg(GPR regnum) const;
    size_t step();
    void write_flags(uint16_t val);
    uint16_t read_flags() const;
    bool has_trapped();
    uint32_t idcode();

    uint16_t debug_run_proc(unsigned addr, bool block = true);
    void debug_seize();
    void debug_step();
    bool debug_is_stopped() const;

    bool instruction_had_side_effects() const
    {
        return false;
    }

    void clear_side_effects()
    {
    }

    void write_mem8(uint16_t segment, uint16_t addr, uint8_t val);
    void write_mem16(uint16_t segment, uint16_t addr, uint16_t val);
    void write_mem32(uint16_t segment, uint16_t addr, uint32_t val);
    uint8_t read_mem8(uint16_t segment, uint16_t addr);
    uint16_t read_mem16(uint16_t segment, uint16_t addr);
    uint32_t read_mem32(uint16_t segment, uint16_t addr);
    void write_vector8(uint16_t segment,
                       uint16_t addr,
                       const std::vector<uint8_t> &v);
    void write_vector16(uint16_t segment,
                        uint16_t addr,
                        const std::vector<uint16_t> &v);
    void write_io8(uint32_t addr, uint8_t val);
    void write_io16(uint32_t addr, uint16_t val);
    uint8_t read_io8(uint32_t addr);
    uint16_t read_io16(uint32_t addr);

    virtual bool has_instruction_length() const
    {
        return false;
    }

    void add_ioport(IOPorts *p)
    {
        (void)p;
        // No emulated IO for hardware, those tests don't run.
    }

    virtual void raise_nmi()
    {
        write_io8(0xfff6, 0x00);
        write_io8(0xfff6, 0x80);
    }

    virtual void raise_irq(int irq_num)
    {
        assert(irq_num >= 0 && irq_num < 7);
        write_io8(0xfff6, 1 << irq_num);
        write_io8(0xfff6, 0);
    }

    unsigned long cycle_count() const
    {
        return num_cycles;
    }

private:
    uint16_t read_ip();
    uint16_t read_sr(GPR regnum);
    uint16_t read_gpr(GPR regnum);
    void write_ip(uint16_t v);
    void write_sr(GPR regnum, uint16_t v);
    void write_gpr(GPR regnum, uint16_t v);
    void debug_write_data(uint16_t v);
    uint16_t debug_read_data();
    void write_mar(uint16_t v);
    void write_mdr(uint16_t v);
    void write_scr(uint16_t v);
    uint16_t read_scr();
    std::string test_name;
    unsigned long num_cycles;
};
