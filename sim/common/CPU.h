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
};

