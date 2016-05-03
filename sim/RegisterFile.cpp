#include "RegisterFile.h"

#include <cstdlib>
#include <cassert>

RegisterFile::RegisterFile()
{
    reset();
}

void RegisterFile::reset()
{
    for (int i = 0; i < 8; ++i)
        registers[i] = 0;

    // Bit 15 is always 1 on 8086
    flags = 0x8000;
}

void RegisterFile::set(GPR regnum, uint16_t value)
{
    assert(regnum < NUM_REGS);

    switch (regnum) {
    case AL:
        registers[AX] &= 0xff00;
        registers[AX] |= value & 0x00ff;
        break;
    case CL:
        registers[CX] &= 0xff00;
        registers[CX] |= value & 0x00ff;
        break;
    case DL:
        registers[DX] &= 0xff00;
        registers[DX] |= value & 0x00ff;
        break;
    case BL:
        registers[BX] &= 0xff00;
        registers[BX] |= value & 0x00ff;
        break;
    case AH:
        registers[AX] &= 0x00ff;
        registers[AX] |= value << 8;
        break;
    case CH:
        registers[CX] &= 0x00ff;
        registers[CX] |= value << 8;
        break;
    case DH:
        registers[DX] &= 0x00ff;
        registers[DX] |= value << 8;
        break;
    case BH:
        registers[BX] &= 0x00ff;
        registers[BX] |= value << 8;
        break;
    case 0 ... NUM_16BIT_REGS:
        registers[regnum] = value;
        break;
    default:
        abort();
    }
}

uint16_t RegisterFile::get(GPR regnum) const
{
    assert(regnum < NUM_REGS);

    switch (regnum) {
    case AL:
        return registers[AX] & 0x00ff;
    case CL:
        return registers[CX] & 0x00ff;
    case DL:
        return registers[DX] & 0x00ff;
    case BL:
        return registers[BX] & 0x00ff;
    case AH:
        return registers[AX] >> 8;
    case CH:
        return registers[CX] >> 8;
    case DH:
        return registers[DX] >> 8;
    case BH:
        return registers[BX] >> 8;
    case 0 ... NUM_16BIT_REGS:
        return registers[regnum];
    default:
        abort();
    }
}

uint16_t RegisterFile::get_flags() const
{
    return flags;
}

void RegisterFile::set_flags(uint16_t val)
{
    flags = val | (1 << 15);
}
