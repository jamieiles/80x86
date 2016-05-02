#include "Memory.h"

#include <cassert>
#include <cstring>

Memory::Memory()
{
    memset(mem, 0, sizeof(mem));
}

template <typename T>
void Memory::write(phys_addr addr, T val)
{
    assert(addr + sizeof(T) <= MEMORY_SIZE);

    memcpy(mem + addr, &val, sizeof(val));
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
