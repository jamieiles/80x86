#pragma once

#include <stddef.h>
#include <stdint.h>
#include <functional>
#include <memory>
#include <map>

#include "CPU.h"
#include "Memory.h"
#include "ModRM.h"

class EmulatorPimpl;
class Memory;
class RegisterFile;
class SoftwareCPU;

class Emulator
{
public:
    Emulator(RegisterFile *registers, SoftwareCPU *cpu);
    virtual ~Emulator();
    size_t step();
    size_t step_with_io(std::function<void(unsigned long)> io_callback);
    void set_memory(Memory *mem);
    void set_io(std::map<uint16_t, IOPorts *> *io);
    bool has_trapped() const;
    void reset();
    void raise_nmi();
    void raise_irq(int irq_num);
    unsigned long cycle_count() const;

private:
    std::unique_ptr<EmulatorPimpl> pimpl;
    unsigned long num_cycles;
};
