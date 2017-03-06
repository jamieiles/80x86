#pragma once

#include "Emulate.h"
#include "Memory.h"
#include "RegisterFile.h"

class CPU {
public:
    virtual ~CPU() {};
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

class SoftwareCPU : public CPU {
public:
    SoftwareCPU()
        : CPU(), emulator(&registers)
    {
        emulator.set_memory(&mem);
        emulator.set_io(&io);
    }

    void write_reg(GPR regnum, uint16_t val)
    {
        registers.set(regnum, val);
    }

    uint16_t read_reg(GPR regnum)
    {
        return registers.get(regnum);
    }

    size_t step()
    {
        return emulator.emulate();
    }

    void write_flags(uint16_t val)
    {
        registers.set_flags(val);
    }

    uint16_t read_flags()
    {
        return registers.get_flags();
    }

    bool has_trapped() const
    {
        return emulator.has_trapped();
    }

    void reset()
    {
        emulator.reset();
    }

    bool instruction_had_side_effects() const
    {
        if (registers.has_written())
            return true;
        return CPU::instruction_had_side_effects();
    }

    void clear_side_effects()
    {
        registers.clear_has_written();
        CPU::clear_side_effects();
    }
private:
    RegisterFile registers;
    Emulator emulator;
};
