#include "RegisterFile.h"

#include <cstdlib>
#include <cassert>

RegisterFile::RegisterFile()
{
    reset();
}

void RegisterFile::reset()
{
    for (int i = 0; i < NUM_16BIT_REGS; ++i)
        registers[i] = 0;

    flags = FLAGS_STUCK_BITS;
    written = false;
}

void RegisterFile::set(GPR regnum, uint16_t value)
{
    assert(regnum < NUM_REGS);

    if (regnum != IP)
        written = false;

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
    case 0 ... NUM_16BIT_REGS - 1: registers[regnum] = value; break;
    default: __builtin_unreachable();
    }
}

uint16_t RegisterFile::get(GPR regnum) const
{
    assert(regnum < NUM_REGS);

    switch (regnum) {
    case AL: return registers[AX] & 0x00ff;
    case CL: return registers[CX] & 0x00ff;
    case DL: return registers[DX] & 0x00ff;
    case BL: return registers[BX] & 0x00ff;
    case AH: return registers[AX] >> 8;
    case CH: return registers[CX] >> 8;
    case DH: return registers[DX] >> 8;
    case BH: return registers[BX] >> 8;
    case 0 ... NUM_16BIT_REGS - 1: return registers[regnum];
    default: __builtin_unreachable();
    }
}

uint16_t RegisterFile::get_flags() const
{
    return flags | FLAGS_STUCK_BITS;
}

bool RegisterFile::get_flag(enum Flag f) const
{
    return !!(flags & f);
}

void RegisterFile::set_flags(uint16_t val, uint16_t mask)
{
    const uint16_t reserved_mask = (1 << 1) | (1 << 3) | (1 << 5);
    const uint16_t valid_mask = (CF | PF | AF | ZF | SF | TF | IF | DF | OF);
    mask &= ~reserved_mask;
    mask &= valid_mask;
    flags &= ~mask;
    flags |= (val & mask) | FLAGS_STUCK_BITS;

    written = true;
}

bool RegisterFile::has_written() const
{
    return written;
}

void RegisterFile::clear_has_written()
{
    written = false;
}
