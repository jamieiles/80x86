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

// xchg r, r/m, 8-bit
void EmulatorPimpl::xchg86()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    auto v1 = read_data<uint8_t>();
    auto v2 = registers->get(modrm_decoder->reg());

    write_data<uint8_t>(v2);
    registers->set(modrm_decoder->reg(), v1);
}

// xchg r, r/m, 16-bit
void EmulatorPimpl::xchg87()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    auto v1 = read_data<uint16_t>();
    auto v2 = registers->get(modrm_decoder->reg());

    write_data<uint16_t>(v2);
    registers->set(modrm_decoder->reg(), v1);
}

// xchg accumulator, r
void EmulatorPimpl::xchg90_97()
{
    auto reg = static_cast<GPR>(static_cast<int>(AX) + (opcode & 0x7));

    auto v1 = registers->get(AX);
    auto v2 = registers->get(reg);

    registers->set(AX, v2);
    registers->set(reg, v1);
}
