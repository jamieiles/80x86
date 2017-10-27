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

void EmulatorPimpl::calle8()
{
    auto displacement = fetch_16bit();

    push_word(registers->get(IP) + instr_length);

    registers->set(IP, registers->get(IP) + instr_length + displacement);
    jump_taken = true;
}

void EmulatorPimpl::callff_intra()
{
    auto displacement = read_data<uint16_t>();

    push_word(registers->get(IP) + instr_length);

    registers->set(IP, displacement);
    jump_taken = true;
}

void EmulatorPimpl::callff_inter()
{
    if (modrm_decoder->rm_type() == OP_REG)
        return;

    push_word(registers->get(CS));
    push_word(registers->get(IP) + instr_length);

    auto cs_ip = read_data<uint32_t>();

    registers->set(IP, cs_ip & 0xffff);
    registers->set(CS, (cs_ip >> 16) & 0xffff);
    jump_taken = true;
}

void EmulatorPimpl::call9a()
{
    auto new_ip = fetch_16bit();
    auto new_cs = fetch_16bit();

    push_word(registers->get(CS));
    push_word(registers->get(IP) + instr_length);

    registers->set(CS, new_cs);
    registers->set(IP, new_ip);
    jump_taken = true;
}
