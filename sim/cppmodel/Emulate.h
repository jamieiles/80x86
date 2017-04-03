#pragma once

#include <stddef.h>
#include <stdint.h>
#include <functional>
#include <memory>

#include "Memory.h"
#include "ModRM.h"

class EmulatorPimpl;
class Memory;
class RegisterFile;

class Emulator {
public:
    Emulator(RegisterFile *registers);
    virtual ~Emulator();
    size_t emulate();
    void set_memory(Memory *mem);
    void set_io(Memory *io);
    bool has_trapped() const;
    void reset();

private:
    std::unique_ptr<EmulatorPimpl> pimpl;
};
