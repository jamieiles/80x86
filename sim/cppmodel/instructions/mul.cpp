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

// mul r/m, 8-bit
void EmulatorPimpl::mulf6()
{
    auto v1 = read_data<uint8_t>();
    auto v2 = registers->get(AL);

    uint16_t result, flags;
    std::tie(flags, result) = do_mul<uint16_t>(v1, v2);

    flags &= ~(CF | OF | ZF);
    if (result & 0xff00)
        flags |= CF | OF;

    registers->set(AX, result);
    registers->set_flags(flags, OF | CF | ZF);
}

// mul r/m, 16-bit
void EmulatorPimpl::mulf7()
{
    auto v1 = read_data<uint16_t>();
    auto v2 = registers->get(AX);

    uint32_t result;
    uint16_t flags;
    std::tie(flags, result) = do_mul<uint32_t>(v1, v2);

    flags &= ~(CF | OF | ZF);
    if (result & 0xffff0000)
        flags |= CF | OF;

    registers->set(AX, result & 0xffff);
    registers->set(DX, (result >> 16) & 0xffff);
    registers->set_flags(flags, OF | CF | ZF);
}
