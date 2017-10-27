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

#include <stdint.h>
#include <functional>
#include <functional>

#include "RegisterFile.h"

enum OperandType {
    OP_REG,
    OP_MEM,
};

enum OperandWidth { OP_WIDTH_8, OP_WIDTH_16 };

class ModRMDecoder
{
public:
    ModRMDecoder(std::function<uint8_t()> get_byte,
                 const RegisterFile *registers);
    void clear();
    void set_width(OperandWidth width);
    void decode();
    GPR reg() const;
    int raw_reg() const;
    GPR rm_reg() const;
    uint16_t effective_address();
    OperandType rm_type() const;
    bool uses_bp_as_base() const;

private:
    uint16_t calculate_effective_address();
    uint16_t mod00();
    uint16_t mod01();
    uint16_t mod10();
    std::function<uint8_t()> get_byte;
    const RegisterFile *registers;
    OperandWidth width;
    uint8_t modrm;
    uint16_t cached_address;
    bool addr_is_cached;
    bool is_decoded;
};
