#pragma once
#include "CPU.h"
#include "Emulate.h"

class SoftwareCPU : public CPU {
public:
    SoftwareCPU()
        : SoftwareCPU("default")
    {
    }
    SoftwareCPU(const std::string &name)
        : CPU(), emulator(&registers)
    {
        (void)name;

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
