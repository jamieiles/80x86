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

#include "Memory.h"

#include <cassert>
#include <cstring>

Memory::Memory()
{
    memset(mem, mem_init_8, sizeof(mem));
    memset(mem + 0x1000, 0, 128);
}

template <typename T>
void Memory::write(phys_addr addr, T val)
{
    assert(addr + sizeof(T) <= MEMORY_SIZE);

    memcpy(mem + addr, &val, sizeof(val));

    written = true;
}
template void Memory::write<uint8_t>(phys_addr addr, uint8_t val);
template void Memory::write<uint16_t>(phys_addr addr, uint16_t val);
template void Memory::write<uint32_t>(phys_addr addr, uint32_t val);

template <typename T>
T Memory::read(phys_addr addr) const
{
    assert(addr + sizeof(T) <= MEMORY_SIZE);

    T val;
    memcpy(&val, mem + addr, sizeof(val));

    return val;
}
template uint8_t Memory::read<uint8_t>(phys_addr addr) const;
template uint16_t Memory::read<uint16_t>(phys_addr addr) const;
template uint32_t Memory::read<uint32_t>(phys_addr addr) const;

bool Memory::has_written() const
{
    return written;
}

void Memory::clear_has_written()
{
    written = false;
}
