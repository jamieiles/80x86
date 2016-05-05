#include "ModRM.h"

#include <cassert>

ModRMDecoder::ModRMDecoder(std::function<uint8_t()> get_byte,
                           const RegisterFile *registers)
        : get_byte(get_byte),
        registers(registers),
        width(OP_WIDTH_16),
        modrm(0)
{
}

void ModRMDecoder::decode()
{
    modrm = get_byte();
}

OperandType ModRMDecoder::rm_type() const
{
    return (modrm & 0xc0) == 0xc0 ? OP_REG : OP_MEM;
}

void ModRMDecoder::set_width(OperandWidth width)
{
    this->width = width;
}

GPR ModRMDecoder::reg() const
{
    int reg = (modrm >> 3) & 0x7;

    if (width == OP_WIDTH_8)
        return static_cast<GPR>(static_cast<int>(AL) + reg);

    return static_cast<GPR>(static_cast<int>(AX) + reg);
}

GPR ModRMDecoder::rm_reg() const
{
    assert(rm_type() == OP_REG);

    int reg = (modrm >> 0) & 0x7;

    if (width == OP_WIDTH_8)
        return static_cast<GPR>(static_cast<int>(AL) + reg);

    return static_cast<GPR>(static_cast<int>(AX) + reg);
}

uint16_t ModRMDecoder::effective_address() const
{
    assert(rm_type() == OP_MEM);

    switch (modrm >> 6) {
    case 0x00: {
        switch (modrm & 0x7) {
        case 0x0: return registers->get(BX) + registers->get(SI);
        case 0x1: return registers->get(BX) + registers->get(DI);
        case 0x2: return registers->get(BP) + registers->get(SI);
        case 0x3: return registers->get(BP) + registers->get(DI);
        case 0x4: return registers->get(SI);
        case 0x5: return registers->get(DI);
        case 0x6: return static_cast<uint16_t>(get_byte()) | (static_cast<uint16_t>(get_byte()) << 8);
        case 0x7: return registers->get(BX);
        }
    }
    case 0x01: {
        auto displacement = static_cast<int8_t>(get_byte());
        switch (modrm & 0x7) {
        case 0x0: return registers->get(BX) + registers->get(SI) + displacement;
        case 0x1: return registers->get(BX) + registers->get(DI) + displacement;
        case 0x2: return registers->get(BP) + registers->get(SI) + displacement;
        case 0x3: return registers->get(BP) + registers->get(DI) + displacement;
        case 0x4: return registers->get(SI) + displacement;
        case 0x5: return registers->get(DI) + displacement;
        case 0x6: return registers->get(BP) + displacement;
        case 0x7: return registers->get(BX) + displacement;
        }
    }
    case 0x02: {
        auto displacement = static_cast<int16_t>(get_byte()) | (static_cast<uint16_t>(get_byte()) << 8);
        switch (modrm & 0x7) {
        case 0x0: return registers->get(BX) + registers->get(SI) + displacement;
        case 0x1: return registers->get(BX) + registers->get(DI) + displacement;
        case 0x2: return registers->get(BP) + registers->get(SI) + displacement;
        case 0x3: return registers->get(BP) + registers->get(DI) + displacement;
        case 0x4: return registers->get(SI) + displacement;
        case 0x5: return registers->get(DI) + displacement;
        case 0x6: return registers->get(BP) + displacement;
        case 0x7: return registers->get(BX) + displacement;
        }
    }
    default:
        abort();
    }
}

bool ModRMDecoder::uses_bp_as_base() const
{
    assert(rm_type() == OP_MEM);

    auto mod = modrm >> 6;
    auto rm = modrm & 0x7;

    if (mod == 0)
        return rm == 0x2 || rm == 0x3;

    return rm == 0x2 || rm == 0x3 || rm == 0x6;
}
