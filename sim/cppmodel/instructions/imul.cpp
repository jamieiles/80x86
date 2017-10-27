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

// imul r/m, 8-bit
void EmulatorPimpl::imulf6()
{
    auto old_flags = registers->get_flags();
    int16_t v1 = sign_extend<int16_t, uint8_t>(read_data<uint8_t>());
    int16_t v2 = sign_extend<int16_t, uint8_t>(registers->get(AL));

    uint16_t result, flags;
    std::tie(flags, result) = do_mul<int16_t>(v1, v2);
    flags = old_flags & ~(CF | OF | ZF);
    if (result & 0xff00)
        flags |= CF | OF;

    registers->set(AX, result);
    registers->set_flags(flags, OF | CF | ZF);
}

// imul r/m, 16-bit
void EmulatorPimpl::imulf7()
{
    auto old_flags = registers->get_flags();
    int32_t v1 = sign_extend<int32_t, uint16_t>(read_data<uint16_t>());
    int32_t v2 = sign_extend<int32_t, uint16_t>(registers->get(AX));

    uint32_t result;
    uint16_t flags;
    std::tie(flags, result) = do_mul<int32_t>(v1, v2);
    flags = old_flags & ~(CF | OF | ZF);
    if (result & 0xffff0000)
        flags |= CF | OF;

    registers->set(AX, result & 0xffff);
    registers->set(DX, (result >> 16) & 0xffff);
    registers->set_flags(flags, OF | CF | ZF);
}

// imul r/m, 16-bit immediate
void EmulatorPimpl::imul69()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    auto old_flags = registers->get_flags();
    int32_t v1 = sign_extend<int32_t, int16_t>(read_data<uint16_t>());
    int32_t v2 = sign_extend<int32_t, int16_t>(fetch_16bit());

    uint32_t result;
    uint16_t flags;
    std::tie(flags, result) = do_mul<int32_t>(v1, v2);
    flags = old_flags & ~(CF | OF | ZF);
    if (result & 0xffff0000)
        flags |= CF | OF;

    registers->set(modrm_decoder->reg(), result & 0xffff);
    registers->set_flags(flags, OF | CF | ZF);
}

// imul r/m, 8-bit immediate
void EmulatorPimpl::imul6b()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    auto old_flags = registers->get_flags();
    int32_t v1 = sign_extend<int32_t, int16_t>(read_data<uint16_t>());
    int32_t v2 = sign_extend<int32_t, int8_t>(fetch_byte());

    uint32_t result;
    uint16_t flags;
    std::tie(flags, result) = do_mul<int32_t>(v1, v2);
    flags = old_flags & ~(CF | OF | ZF);
    if (result & 0xffff0000)
        flags |= CF | OF;

    registers->set(modrm_decoder->reg(), result & 0xffff);
    registers->set_flags(flags, OF | CF | ZF);
}
