#pragma once

#include <memory>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

#include "Memory.h"
#include "RegisterFile.h"

#define __unused __attribute__((unused))

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

static inline phys_addr get_phys_addr(uint16_t segment,
                                      uint32_t displacement)
{
    return ((static_cast<uint32_t>(segment) << 4) + displacement) % (1 * 1024 * 1024);
}

class IOPorts {
public:
    IOPorts(uint16_t base, size_t num_ports)
        : base(base),
        num_ports(num_ports)
    {}
    virtual uint8_t read8(uint16_t port_num,
                          unsigned offs) = 0;
    virtual uint16_t read16(uint16_t port_num)
    {
        return read8(port_num, 0) | (static_cast<uint16_t>(read8(port_num, 1)) << 8);
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

class CPU {
public:
    CPU()
        : CPU("default")
    {
    }
    CPU(const std::string __unused &name)
    {
    }
    virtual ~CPU() {};
    virtual bool has_instruction_length() const = 0;
    virtual void write_coverage() {}
    virtual void write_reg(GPR regnum, uint16_t val) = 0;
    virtual uint16_t read_reg(GPR regnum) = 0;
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
    virtual uint16_t read_flags() = 0;
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

    virtual void write_vector8(uint16_t segment, uint16_t addr, const std::vector<uint8_t> &v)
    {
        uint16_t offs = 0;
        for (auto &b: v)
            write_mem8(segment, addr + offs++, b);
    }
    virtual void write_vector16(uint16_t segment, uint16_t addr, const std::vector<uint16_t> &v)
    {
        uint16_t offs = 0;
        for (auto &w: v) {
            write_mem16(segment, addr + offs, w);
            offs += 2;
        }
    }
    virtual void raise_nmi() = 0;
    virtual void raise_irq(int irq_num) = 0;
    virtual void clear_irq(int irq_num) = 0;
    virtual unsigned long cycle_count() const = 0;
};

class SimCPU : public CPU {
public:
    SimCPU(const std::string &name)
        : CPU(name)
    {}

    Memory *get_memory()
    {
        return &mem;
    }

    virtual size_t step_with_io(std::function<void(unsigned long)> io_callback)
    {
        (void)io_callback;

        throw NotImplemented("step_with_io not implemented");
    }

    virtual void cycle_cpu_with_io(std::function<void(unsigned long)> io_callback)
    {
        (void)io_callback;

        throw NotImplemented("cycle_with_io not implemented");
    }
protected:
    Memory mem;
};
