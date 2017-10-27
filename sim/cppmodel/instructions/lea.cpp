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

// lea r, r/m
void EmulatorPimpl::lea8d()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    if (modrm_decoder->rm_type() != OP_REG)
        registers->set(modrm_decoder->reg(),
                       modrm_decoder->effective_address());
}

// lds r, m
void EmulatorPimpl::ldsc5()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    if (modrm_decoder->rm_type() == OP_REG)
        return;

    auto p32 = get_phys_addr(registers->get(get_segment(false)),
                             modrm_decoder->effective_address());
    auto displacement = mem->read<uint16_t>(p32);
    auto seg = mem->read<uint16_t>(p32 + 2);

    registers->set(modrm_decoder->reg(), displacement);
    registers->set(DS, seg);
}

// les r, m
void EmulatorPimpl::lesc4()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    if (modrm_decoder->rm_type() == OP_REG)
        return;

    auto p32 = get_phys_addr(registers->get(get_segment(false)),
                             modrm_decoder->effective_address());
    auto displacement = mem->read<uint16_t>(p32);
    auto seg = mem->read<uint16_t>(p32 + 2);

    registers->set(modrm_decoder->reg(), displacement);
    registers->set(ES, seg);
}
