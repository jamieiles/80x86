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
#include <map>

#include "CPU.h"
#include "Emulate.h"

class Emulator;
class EmulatorPimpl;

class SoftwareCPU : public SimCPU
{
public:
    SoftwareCPU() : SoftwareCPU("default")
    {
    }
    SoftwareCPU(const std::string &name)
        : SimCPU(name), emulator(&registers, this)
    {
        (void)name;

        emulator.set_memory(&mem);
        emulator.set_io(&io);
    }

    void write_reg(GPR regnum, uint16_t val)
    {
        registers.set(regnum, val);
    }

    uint16_t read_reg(GPR regnum) const
    {
        return registers.get(regnum);
    }

    size_t step()
    {
        return emulator.step();
    }

    size_t step_with_io(std::function<void(unsigned long)> io_callback)
    {
        return emulator.step_with_io(io_callback);
    }

    void write_flags(uint16_t val)
    {
        registers.set_flags(val);
    }

    uint16_t read_flags() const
    {
        return registers.get_flags();
    }

    bool has_trapped()
    {
        return emulator.has_trapped();
    }

    void reset()
    {
        emulator.reset();
    }

    bool instruction_had_side_effects() const
    {
        return mem.has_written() || registers.has_written();
    }

    void clear_side_effects()
    {
        registers.clear_has_written();
        mem.clear_has_written();
    }

    void write_mem8(uint16_t segment, uint16_t addr, uint8_t val)
    {
        mem.write<uint8_t>(get_phys_addr(segment, addr), val);
    }
    void write_mem16(uint16_t segment, uint16_t addr, uint16_t val)
    {
        mem.write<uint16_t>(get_phys_addr(segment, addr), val);
    }
    void write_mem32(uint16_t segment, uint16_t addr, uint32_t val)
    {
        mem.write<uint32_t>(get_phys_addr(segment, addr), val);
    }

    uint8_t read_mem8(uint16_t segment, uint16_t addr)
    {
        return mem.read<uint8_t>(get_phys_addr(segment, addr));
    }
    uint16_t read_mem16(uint16_t segment, uint16_t addr)
    {
        return mem.read<uint16_t>(get_phys_addr(segment, addr));
    }
    uint32_t read_mem32(uint16_t segment, uint16_t addr)
    {
        return mem.read<uint32_t>(get_phys_addr(segment, addr));
    }

    void write_io8(uint32_t addr, uint8_t val)
    {
        if (!io.count(addr & ~1))
            return;

        auto p = io[addr & ~1];
        p->write8((addr & -1) - p->get_base(), addr & 1, val);
    }
    void write_io16(uint32_t addr, uint16_t val)
    {
        if (!io.count(addr & ~1))
            return;

        auto p = io[addr & ~1];
        p->write16((addr & -1) - p->get_base(), val);
    }

    uint8_t read_io8(uint32_t addr)
    {
        if (!io.count(addr & ~1))
            return 0;

        auto p = io[addr & ~1];
        return p->read8((addr & -1) - p->get_base(), addr & 1);
    }
    uint16_t read_io16(uint32_t addr)
    {
        if (!io.count(addr & ~1))
            return 0;

        auto p = io[addr & ~1];
        return p->read16((addr & -1) - p->get_base());
    }

    virtual bool has_instruction_length() const
    {
        return true;
    }

    virtual void add_ioport(IOPorts *p)
    {
        for (size_t m = 0; m < p->get_num_ports(); ++m)
            io[p->get_base() + m * sizeof(uint16_t)] = p;
    }

    virtual void raise_nmi()
    {
        emulator.raise_nmi();
    }

    virtual void raise_irq(int irq_num)
    {
        emulator.raise_irq(irq_num);
    }

    unsigned long cycle_count() const
    {
        return emulator.cycle_count();
    }

private:
    RegisterFile registers;
    Emulator emulator;

    std::map<uint16_t, IOPorts *> io;
};
