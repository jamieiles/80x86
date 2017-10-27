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

// aam
void EmulatorPimpl::aamd4()
{
    auto divisor = fetch_byte();

    if (divisor == 0) {
        auto flags = registers->get_flags();
        push_word(flags);
        push_word(registers->get(CS));
        push_word(registers->get(IP));

        flags &= ~(IF | TF);
        registers->set_flags(flags, IF | TF);

        auto new_cs = mem->read<uint16_t>(VEC_DIVIDE_ERROR + 2);
        auto new_ip = mem->read<uint16_t>(VEC_DIVIDE_ERROR + 0);

        registers->set(CS, new_cs);
        registers->set(IP, new_ip);
        jump_taken = true;

        return;
    }

    auto al = registers->get(AL);
    auto quotient = al / divisor;
    auto remainder = al % divisor;

    registers->set(AH, quotient);
    registers->set(AL, remainder);

    uint16_t flags = registers->get_flags();
    flags &= ~(PF | SF | ZF);

    if (!(registers->get(AX) & 0xffff))
        flags |= ZF;
    if (!__builtin_parity(registers->get(AL) & 0xff))
        flags |= PF;
    if (registers->get(AX) & 0x8000)
        flags |= SF;

    registers->set_flags(flags, SF | ZF | PF);
}
