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

#include <memory>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/serialization/list.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>

#include "Memory.h"
#include "RegisterFile.h"

#define __unused __attribute__((unused))

class Emulator;
class EmulatorPimpl;

enum InterruptVectorOffset {
    VEC_DIVIDE_ERROR = 0,
    VEC_SINGLE_STEP = 4,
    VEC_NMI = 8,
    VEC_INT = 12,
    VEC_OVERFLOW = 16,
    VEC_BOUND = 20,
    VEC_INVALID_OPCODE = 24,
    VEC_ESCAPE = 28
};

static inline phys_addr get_phys_addr(uint16_t segment, uint32_t displacement)
{
    return ((static_cast<uint32_t>(segment) << 4) + displacement) %
           (1 * 1024 * 1024);
}

class IOPorts
{
public:
    IOPorts(uint16_t base, size_t num_ports) : base(base), num_ports(num_ports)
    {
    }

    virtual ~IOPorts()
    {
    }

    virtual uint8_t read8(uint16_t port_num, unsigned offs) = 0;
    virtual uint16_t read16(uint16_t port_num)
    {
        return read8(port_num, 0) |
               (static_cast<uint16_t>(read8(port_num, 1)) << 8);
    }
    virtual void write8(uint16_t port_num, unsigned offs, uint8_t v) = 0;
    virtual void write16(uint16_t port_num, uint16_t v)
    {
        write8(port_num, 0, v);
        write8(port_num, 1, v >> 8);
    }
    uint16_t get_base() const
    {
        return base;
    }
    size_t get_num_ports() const
    {
        return num_ports;
    }

private:
    uint16_t base;
    size_t num_ports;
};

using NotImplemented = std::runtime_error;

class CPU
{
public:
    CPU() : CPU("default")
    {
    }
    explicit CPU(const std::string __unused &name)
    {
    }
    virtual ~CPU(){};
    virtual bool has_instruction_length() const = 0;
    virtual void write_coverage()
    {
    }
    virtual void write_reg(GPR regnum, uint16_t val) = 0;
    virtual uint16_t read_reg(GPR regnum) const = 0;
    virtual void cycle_cpu()
    {
        throw NotImplemented("cycle not implemented");
    }
    virtual void start_instruction()
    {
        throw NotImplemented("start_instruction not implemented");
    }
    virtual void complete_instruction()
    {
        throw NotImplemented("complete_instruction not implemented");
    }
    virtual bool int_yield_ready()
    {
        throw NotImplemented("int_yield_ready not implemented");
    }
    virtual void debug_detach()
    {
    }
    virtual size_t step() = 0;
    virtual void write_flags(uint16_t val) = 0;
    virtual uint16_t read_flags() const = 0;
    virtual bool has_trapped() = 0;
    virtual void reset() = 0;
    virtual bool instruction_had_side_effects() const = 0;
    virtual void clear_side_effects() = 0;
    virtual void write_mem8(uint16_t segment, uint16_t addr, uint8_t val) = 0;
    virtual void write_mem16(uint16_t segment, uint16_t addr, uint16_t val) = 0;
    virtual void write_mem32(uint16_t segment, uint16_t addr, uint32_t val) = 0;
    virtual uint8_t read_mem8(uint16_t segment, uint16_t addr) = 0;
    virtual uint16_t read_mem16(uint16_t segment, uint16_t addr) = 0;
    virtual uint32_t read_mem32(uint16_t segment, uint16_t addr) = 0;
    virtual void write_io8(uint32_t addr, uint8_t val) = 0;
    virtual void write_io16(uint32_t addr, uint16_t val) = 0;
    virtual uint8_t read_io8(uint32_t addr) = 0;
    virtual uint16_t read_io16(uint32_t addr) = 0;
    virtual void add_ioport(IOPorts *p) = 0;

    virtual std::vector<uint8_t> read_vector8(uint16_t segment,
                                              uint16_t addr,
                                              size_t count)
    {
        std::vector<uint8_t> v;

        for (size_t m = 0; m < count; ++m)
            v.push_back(read_mem8(segment, addr + m));

        return v;
    }

    virtual void write_vector8(uint16_t segment,
                               uint16_t addr,
                               const std::vector<uint8_t> &v)
    {
        uint16_t offs = 0;
        for (auto &b : v)
            write_mem8(segment, addr + offs++, b);
    }

    virtual std::vector<uint16_t> read_vector16(uint16_t segment,
                                                uint16_t addr,
                                                size_t count)
    {
        std::vector<uint16_t> v;

        for (size_t m = 0; m < count; ++m)
            v.push_back(read_mem16(segment, addr + m * sizeof(uint16_t)));

        return v;
    }

    virtual void write_vector16(uint16_t segment,
                                uint16_t addr,
                                const std::vector<uint16_t> &v)
    {
        uint16_t offs = 0;
        for (auto &w : v) {
            write_mem16(segment, addr + offs, w);
            offs += 2;
        }
    }
    virtual void raise_nmi() = 0;
    virtual void raise_irq(int irq_num) = 0;
    virtual void set_inta_handler(std::function<void(int)> __unused handler)
    {
        throw NotImplemented("set_inta_handler not implemented");
    }
    virtual unsigned long cycle_count() const = 0;
};

class SimCPU : public CPU
{
public:
    explicit SimCPU(const std::string &name)
        : CPU(name), inta_handler([](int __unused irq_num) {})
    {
    }

    Memory *get_memory()
    {
        return &mem;
    }

    virtual size_t step_with_io(std::function<void(unsigned long)> io_callback)
    {
        (void)io_callback;

        throw NotImplemented("step_with_io not implemented");
    }

    virtual void cycle_cpu_with_io(
        std::function<void(unsigned long)> io_callback)
    {
        (void)io_callback;

        throw NotImplemented("cycle_with_io not implemented");
    }

    virtual void set_inta_handler(std::function<void(int)> handler)
    {
        this->inta_handler = handler;
    }

protected:
    Memory mem;

    void ack_int(int irq_num)
    {
        this->inta_handler(irq_num);
    }

    friend class Emulator;
    friend class EmulatorPimpl;

private:
    std::function<void(int)> inta_handler;

    friend class boost::serialization::access;
    template <class Archive>
    void save(Archive &ar, const unsigned int __unused version) const
    {
        for (auto r = static_cast<int>(AX);
             r <= static_cast<int>(NUM_16BIT_REGS); ++r) {
            auto v = read_reg(static_cast<GPR>(r));
            // clang-format off
            ar & v;
            // clang-format on
        }

        // clang-format off
        auto flags = read_flags();
        ar & flags;

        ar & mem;
        // clang-format on
    }

    template <class Archive>
    void load(Archive &ar, const unsigned int __unused version)
    {
        for (auto r = static_cast<int>(AX);
             r <= static_cast<int>(NUM_16BIT_REGS); ++r) {
            uint16_t v = 0;
            // clang-format off
            ar & v;
            write_reg(static_cast<GPR>(r), v);
            // clang-format on
        }

        // clang-format off
        uint16_t flags = 0;
        ar & flags;
        write_flags(flags);

        ar & mem;
        // clang-format on
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
};
BOOST_CLASS_VERSION(SimCPU, 1)
