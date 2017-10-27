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

#include <vector>

#include "RegisterFile.h"
#include "ModRM.h"

class ModRMDecoderTestBench
{
public:
    virtual void set_instruction(const std::vector<uint8_t> instruction) = 0;
    virtual void decode() = 0;
    virtual OperandType get_rm_type() const = 0;
    virtual uint16_t get_effective_address() const = 0;
    virtual GPR get_register() const = 0;
    virtual GPR get_rm_register() const = 0;
    RegisterFile *get_registers()
    {
        return &regs;
    }

protected:
    RegisterFile regs;
};
