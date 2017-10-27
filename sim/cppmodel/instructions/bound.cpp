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

void EmulatorPimpl::bound62()
{
    modrm_decoder->decode();

    if (modrm_decoder->rm_type() == OP_REG) {
        invalid_opcode();
        return;
    }

    auto p32 = get_phys_addr(registers->get(get_segment(false)),
                             modrm_decoder->effective_address());
    auto lower = static_cast<int16_t>(mem->read<uint16_t>(p32));
    auto upper = static_cast<int16_t>(mem->read<uint16_t>(p32 + 2));

    auto v = static_cast<int16_t>(registers->get(modrm_decoder->reg()));

    if (v >= lower && v <= upper)
        return;

    auto flags = registers->get_flags();
    push_word(flags);
    push_word(registers->get(CS));
    push_word(registers->get(IP) + instr_length);

    flags &= ~(IF | TF);
    registers->set_flags(flags, IF | TF);

    // int 3
    auto new_cs = mem->read<uint16_t>(VEC_BOUND + 2);
    auto new_ip = mem->read<uint16_t>(VEC_BOUND + 0);

    registers->set(CS, new_cs);
    registers->set(IP, new_ip);
    jump_taken = true;
}
