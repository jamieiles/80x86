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

// pop r/m
void EmulatorPimpl::pop8f()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() != 0)
        return;

    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    auto val = mem->read<uint16_t>(addr);
    registers->set(SP, registers->get(SP) + 2);

    write_data<uint16_t>(val);
}

// pop r
void EmulatorPimpl::pop58_5f()
{
    auto reg = static_cast<GPR>(static_cast<int>(AX) + (opcode & 0x7));
    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    auto val = mem->read<uint16_t>(addr);

    registers->set(SP, registers->get(SP) + 2);
    registers->set(reg, val);
}

// pop sr
void EmulatorPimpl::popsr()
{
    auto reg = static_cast<GPR>(static_cast<int>(ES) + ((opcode >> 3) & 0x3));
    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    auto val = mem->read<uint16_t>(addr);

    registers->set(reg, val);
    registers->set(SP, registers->get(SP) + 2);
    ext_int_inhibit = true;
}

// popf
void EmulatorPimpl::popf9d()
{
    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    auto val = mem->read<uint16_t>(addr);
    registers->set_flags(val);

    registers->set(SP, registers->get(SP) + 2);
}
