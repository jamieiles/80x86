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

void EmulatorPimpl::escd8()
{
    modrm_decoder->decode();

    if (!S80X86_TRAP_ESCAPE)
        return;

    auto flags = registers->get_flags();

    push_word(flags);
    push_word(registers->get(CS));
    push_word(registers->get(IP));

    flags &= ~(IF | TF);
    registers->set_flags(flags, IF | TF);

    auto new_cs = mem->read<uint16_t>(VEC_ESCAPE + 2);
    auto new_ip = mem->read<uint16_t>(VEC_ESCAPE + 0);

    registers->set(CS, new_cs);
    registers->set(IP, new_ip);
    jump_taken = true;
}
