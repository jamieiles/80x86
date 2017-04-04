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
    virtual bool has_trapped() = 0;
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

    void write_mem8(uint32_t addr, uint8_t val)
    {
        mem.write<uint8_t>(addr, val);
    }
    void write_mem16(uint32_t addr, uint16_t val)
    {
        mem.write<uint16_t>(addr, val);
    }
    void write_mem32(uint32_t addr, uint32_t val)
    {
        mem.write<uint32_t>(addr, val);
    }

    uint8_t read_mem8(uint32_t addr)
    {
        return mem.read<uint8_t>(addr);
    }
    uint16_t read_mem16(uint32_t addr)
    {
        return mem.read<uint16_t>(addr);
    }
    uint32_t read_mem32(uint32_t addr)
    {
        return mem.read<uint32_t>(addr);
    }

    void write_io8(uint32_t addr, uint8_t val)
    {
        io.write<uint8_t>(addr, val);
    }
    void write_io16(uint32_t addr, uint16_t val)
    {
        io.write<uint16_t>(addr, val);
    }
    void write_io32(uint32_t addr, uint32_t val)
    {
        io.write<uint32_t>(addr, val);
    }

    uint8_t read_io8(uint32_t addr)
    {
        return io.read<uint8_t>(addr);
    }
    uint16_t read_io16(uint32_t addr)
    {
        return io.read<uint16_t>(addr);
    }
    uint32_t read_io32(uint32_t addr)
    {
        return io.read<uint32_t>(addr);
    }

protected:
    Memory mem;
    Memory io;
};

