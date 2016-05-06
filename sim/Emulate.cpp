#include "Emulate.h"

#include <stdint.h>
#include "Fifo.h"
#include "Memory.h"

Emulator::Emulator(RegisterFile *registers)
    : registers(registers)
{
    modrm_decoder = std::make_unique<ModRMDecoder>(
        [&]{ return this->fetch_byte(); },
        this->registers
    );
}

size_t Emulator::emulate()
{
    instr_length = 0;

    opcode = fetch_byte();
    switch (opcode) {
    case 0x88: mov88(); break;
    case 0x89: mov89(); break;
    case 0x8a: mov8a(); break;
    case 0x8b: mov8b(); break;
    case 0xc6: movc6(); break;
    case 0xc7: movc7(); break;
    case 0xb0 ... 0xb7: movb0_b7(); break;
    case 0xb8 ... 0xbf: movb8_bf(); break;
    case 0xa0: mova0(); break;
    case 0xa1: mova1(); break;
    case 0xa2: mova2(); break;
    case 0xa3: mova3(); break;
    case 0x8e: mov8e(); break;
    case 0x8c: mov8c(); break;
    }

    return instr_length;
}

// mov m/r, r (8-bit)
void Emulator::mov88()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    auto source = modrm_decoder->reg();
    auto val = registers->get(source);

    write_data<uint8_t>(val);
}

// mov m/r, r (16-bit)
void Emulator::mov89()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    auto source = modrm_decoder->reg();
    auto val = registers->get(source);

    write_data<uint16_t>(val);
}

// mov r, m/r (8-bit)
void Emulator::mov8a()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t val = read_data<uint8_t>();

    auto dest = modrm_decoder->reg();
    registers->set(dest, val);
}

// mov r, m/r (16-bit)
void Emulator::mov8b()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t val = read_data<uint16_t>();

    auto dest = modrm_decoder->reg();
    registers->set(dest, val);
}

// mov r/m, immediate (reg == 0), 8-bit
void Emulator::movc6()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() == 0) {
        uint8_t immed = fetch_byte();
        write_data<uint8_t>(immed);
    }
}

// mov r/m, immediate (reg == 0), 16-bit
void Emulator::movc7()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() == 0)
        write_data<uint16_t>(fetch_16bit());
}

// mov r, immediate, 8-bit
void Emulator::movb0_b7()
{
    uint8_t immed = fetch_byte();
    auto reg = static_cast<GPR>(static_cast<int>(AL) + (opcode & 0x7));
    registers->set(reg, immed);
}

// mov r, immediate, 16-bit
void Emulator::movb8_bf()
{
    uint16_t immed = fetch_16bit();
    auto reg = static_cast<GPR>(static_cast<int>(AX) + (opcode & 0x7));
    registers->set(reg, immed);
}

// mov al, m, 8-bit
void Emulator::mova0()
{
    auto displacement = fetch_16bit();
    auto val = mem->read<uint8_t>((registers->get(DS) << 4) + displacement);
    registers->set(AL, val);
}

// mov ax, m, 16-bit
void Emulator::mova1()
{
    auto displacement = fetch_16bit();
    auto val = mem->read<uint16_t>((registers->get(DS) << 4) + displacement);
    registers->set(AX, val);
}

// mov m, al 8-bit
void Emulator::mova2()
{
    auto displacement = fetch_16bit();
    auto val = registers->get(AL);
    mem->write<uint8_t>((registers->get(DS) << 4) + displacement, val);
}

// mov m, al 16-bit
void Emulator::mova3()
{
    auto displacement = fetch_16bit();
    auto val = registers->get(AX);
    mem->write<uint16_t>((registers->get(DS) << 4) + displacement, val);
}

// mov sr, r/m
void Emulator::mov8e()
{
    if (modrm_decoder->raw_reg() & (1 << 4))
        return;

    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t val = read_data<uint16_t>();
    auto segnum = modrm_decoder->raw_reg();
    auto reg = static_cast<GPR>(static_cast<int>(ES) + segnum);

    registers->set(reg, val);
}

// mov r/m, sr
void Emulator::mov8c()
{
    if (modrm_decoder->raw_reg() & (1 << 4))
        return;

    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    auto segnum = modrm_decoder->raw_reg();
    auto reg = static_cast<GPR>(static_cast<int>(ES) + segnum);
    uint16_t val = registers->get(reg);

    write_data<uint16_t>(val);
}

uint8_t Emulator::fetch_byte()
{
    ++instr_length;

    return instr_stream->pop();
}

uint16_t Emulator::fetch_16bit()
{
    uint16_t immed = (static_cast<uint16_t>(fetch_byte()) |
                      (static_cast<uint16_t>(fetch_byte()) << 8));
    return immed;
}

template <typename T>
void Emulator::write_data(T val)
{
    if (modrm_decoder->rm_type() == OP_REG) {
        auto dest = modrm_decoder->rm_reg();
        registers->set(dest, val);
    } else {
        auto ea = modrm_decoder->effective_address();
        auto segment = modrm_decoder->uses_bp_as_base() ? SS : DS;
        mem->write<T>((registers->get(segment) << 4) + ea, val);
    }
}

template <typename T>
T Emulator::read_data()
{
    if (modrm_decoder->rm_type() == OP_MEM) {
        auto displacement = modrm_decoder->effective_address();
        auto segment = modrm_decoder->uses_bp_as_base() ? SS : DS;

        return mem->read<T>((registers->get(segment) << 4) + displacement);
    } else {
        auto source = modrm_decoder->rm_reg();
        return registers->get(source);
    }
}
