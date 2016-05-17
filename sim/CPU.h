#ifndef __CPU_H__
#define __CPU_H__

#include "Emulate.h"
#include "Memory.h"
#include "RegisterFile.h"

class CPU {
public:
    CPU()
        : emulator(&registers)
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

    template <typename T>
    void write_mem(uint32_t addr, T val)
    {
        mem.write<T>(addr, val);
    }

    template <typename T>
    T read_mem(uint16_t addr)
    {
        return mem.read<T>(addr);
    }

    template <typename T>
    void write_io(uint16_t addr, T val)
    {
        io.write<T>(addr, val);
    }

    template <typename T>
    T read_io(uint16_t addr)
    {
        return io.read<T>(addr);
    }

    size_t cycle()
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
private:
    Memory mem;
    Memory io;
    RegisterFile registers;
    Emulator emulator;
};

#endif /* __CPU_H__ */
