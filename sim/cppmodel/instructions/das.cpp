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

// das
void EmulatorPimpl::das2f()
{
    uint16_t flags = registers->get_flags();

    auto al = registers->get(AL);
    auto ah = registers->get(AH);
    if ((al & 0x0f) > 9 || (flags & AF)) {
        uint16_t tmp = al - 6;
        al = tmp & 0xff;
        flags |= AF;
        if (tmp & 0xff00)
            flags |= CF;
    } else {
        flags &= ~AF;
    }

    if (al > 0x9f || (flags & CF)) {
        al -= 0x60;
        flags |= CF;
    } else {
        flags &= ~CF;
    }

    if (al & 0x80)
        flags |= SF;
    if (!al)
        flags |= ZF;
    if (!__builtin_parity(al))
        flags |= PF;

    registers->set(AL, al);
    registers->set(AH, ah);
    registers->set_flags(flags, SF | ZF | AF | CF | PF);
}
