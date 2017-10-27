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
