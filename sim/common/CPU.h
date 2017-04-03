#pragma once

#include <string>

#include "Memory.h"
#include "RegisterFile.h"

enum InterruptVectorOffset {
    VEC_DIVIDE_ERROR = 0,
    VEC_SINGLE_STEP = 4,
    VEC_NMI = 8,
    VEC_INT = 12,
    VEC_OVERFLOW = 16
};

static inline phys_addr get_phys_addr(uint16_t segment,
                                      uint32_t displacement)
{
    return ((static_cast<uint32_t>(segment) << 4) + displacement) % (1 * 1024 * 1024);
}

class CPU {
public:
    CPU()
        : CPU("default")
    {
    }
    CPU(const std::string &name)
    {
        (void)name;
    }
    virtual ~CPU() {};
    virtual void write_coverage() {}
    virtual void write_reg(GPR regnum, uint16_t val) = 0;
    virtual uint16_t read_reg(GPR regnum) = 0;
    virtual size_t step() = 0;
    virtual void write_flags(uint16_t val) = 0;
    virtual uint16_t read_flags() = 0;
    virtual bool has_trapped() const = 0;
    virtual void reset() = 0;

    virtual bool instruction_had_side_effects() const
    {
        return mem.has_written() || io.has_written();
    }

    virtual void clear_side_effects()
    {
        mem.clear_has_written();
        io.clear_has_written();
    }

    template <typename T>
    void write_mem(uint32_t addr, T val)
    {
        mem.write<T>(addr, val);
    }

    template <typename T>
    T read_mem(uint32_t addr)
    {
        return mem.read<T>(addr);
    }

    template <typename T>
    void write_io(uint32_t addr, T val)
    {
        io.write<T>(addr, val);
    }

    template <typename T>
    T read_io(uint32_t addr)
    {
        return io.read<T>(addr);
    }
protected:
    Memory mem;
    Memory io;
};

