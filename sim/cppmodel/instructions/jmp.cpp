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

void EmulatorPimpl::jmpe9()
{
    int16_t displacement = static_cast<int16_t>(fetch_16bit());

    registers->set(IP, registers->get(IP) + displacement + instr_length);
    jump_taken = true;
}

void EmulatorPimpl::jmpea()
{
    uint16_t ip = fetch_16bit();
    uint16_t cs = fetch_16bit();

    registers->set(IP, ip);
    registers->set(CS, cs);
    jump_taken = true;
}

void EmulatorPimpl::jmpeb()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    registers->set(IP, registers->get(IP) + displacement + instr_length);
    jump_taken = true;
}

void EmulatorPimpl::je74()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (registers->get_flag(ZF)) {
        registers->set(IP, registers->get(IP) + displacement + instr_length);
        jump_taken = true;
    }
}

void EmulatorPimpl::jl7c()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (registers->get_flag(OF) ^ registers->get_flag(SF)) {
        registers->set(IP, registers->get(IP) + displacement + instr_length);
        jump_taken = true;
    }
}

void EmulatorPimpl::jle7e()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if ((registers->get_flag(OF) ^ registers->get_flag(SF)) ||
        registers->get_flag(ZF)) {
        registers->set(IP, registers->get(IP) + displacement + instr_length);
        jump_taken = true;
    }
}

void EmulatorPimpl::jb72()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (registers->get_flag(CF)) {
        registers->set(IP, registers->get(IP) + displacement + instr_length);
        jump_taken = true;
    }
}

void EmulatorPimpl::jbe76()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (registers->get_flag(CF) || registers->get_flag(ZF)) {
        registers->set(IP, registers->get(IP) + displacement + instr_length);
        jump_taken = true;
    }
}

void EmulatorPimpl::jp7a()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (registers->get_flag(PF)) {
        registers->set(IP, registers->get(IP) + displacement + instr_length);
        jump_taken = true;
    }
}

void EmulatorPimpl::jo70()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (registers->get_flag(OF)) {
        registers->set(IP, registers->get(IP) + displacement + instr_length);
        jump_taken = true;
    }
}

void EmulatorPimpl::js78()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (registers->get_flag(SF)) {
        registers->set(IP, registers->get(IP) + displacement + instr_length);
        jump_taken = true;
    }
}

void EmulatorPimpl::jne75()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (!registers->get_flag(ZF)) {
        registers->set(IP, registers->get(IP) + displacement + instr_length);
        jump_taken = true;
    }
}

void EmulatorPimpl::jnl7d()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (!(registers->get_flag(SF) ^ registers->get_flag(OF))) {
        registers->set(IP, registers->get(IP) + displacement + instr_length);
        jump_taken = true;
    }
}

void EmulatorPimpl::jnle7f()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (((registers->get_flag(SF) ^ registers->get_flag(OF)) |
         registers->get_flag(ZF)) == 0) {
        registers->set(IP, registers->get(IP) + displacement + instr_length);
        jump_taken = true;
    }
}

void EmulatorPimpl::jnb73()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (!registers->get_flag(CF)) {
        registers->set(IP, registers->get(IP) + displacement + instr_length);
        jump_taken = true;
    }
}

void EmulatorPimpl::jnbe77()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if ((registers->get_flag(CF) || registers->get_flag(ZF)) == 0) {
        registers->set(IP, registers->get(IP) + displacement + instr_length);
        jump_taken = true;
    }
}

void EmulatorPimpl::jnp7b()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (!registers->get_flag(PF)) {
        registers->set(IP, registers->get(IP) + displacement + instr_length);
        jump_taken = true;
    }
}

void EmulatorPimpl::jno71()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (!registers->get_flag(OF)) {
        registers->set(IP, registers->get(IP) + displacement + instr_length);
        jump_taken = true;
    }
}

void EmulatorPimpl::jmpff_intra()
{
    auto ip = read_data<uint16_t>();

    registers->set(IP, ip);
    jump_taken = true;
}

void EmulatorPimpl::jmpff_inter()
{
    if (modrm_decoder->rm_type() == OP_REG)
        return;

    auto cs_ip = read_data<uint32_t>();

    registers->set(IP, cs_ip & 0xffff);
    registers->set(CS, (cs_ip >> 16) & 0xffff);
    jump_taken = true;
}

void EmulatorPimpl::jns79()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (!registers->get_flag(SF)) {
        registers->set(IP, registers->get(IP) + displacement + instr_length);
        jump_taken = true;
    }
}

void EmulatorPimpl::jcxze3()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (!registers->get(CX)) {
        registers->set(IP, registers->get(IP) + displacement + instr_length);
        jump_taken = true;
    }
}
