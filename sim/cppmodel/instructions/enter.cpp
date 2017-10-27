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

void EmulatorPimpl::enterc8()
{
    auto size = fetch_16bit();
    auto nest = fetch_byte() & 0x1f;

    push_word(registers->get(BP));
    auto frame_temp = registers->get(SP);

    for (int i = 1; i < nest; ++i) {
        registers->set(BP, registers->get(BP) - 2);
        push_word(mem->read<uint16_t>(
            get_phys_addr(registers->get(SS), registers->get(BP))));
    }

    if (nest)
        push_word(frame_temp);

    registers->set(BP, frame_temp);
    registers->set(SP, registers->get(SP) - size);
}
