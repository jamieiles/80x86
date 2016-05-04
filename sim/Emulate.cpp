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

    auto opcode = fetch_byte();
    switch (opcode) {
    case 0x88: mov88(); break;
    case 0x89: mov89(); break;
    case 0x8a: mov8a(); break;
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

    write_result<uint8_t>(val);
}

// mov m/r, r (16-bit)
void Emulator::mov89()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    auto source = modrm_decoder->reg();
    auto val = registers->get(source);

    write_result<uint8_t>(val);
}

// mov r, m/r (8-bit)
void Emulator::mov8a()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t val;
    if (modrm_decoder->rm_type() == OP_MEM) {
        val = mem->read<uint8_t>(modrm_decoder->effective_address());
    } else {
        auto source = modrm_decoder->rm_reg();
        val = registers->get(source);
    }

    auto dest = modrm_decoder->reg();
    registers->set(dest, val);
}

uint8_t Emulator::fetch_byte()
{
    ++instr_length;

    return instr_stream->pop();
}

template <typename T>
void Emulator::write_result(T val)
{
    if (modrm_decoder->rm_type() == OP_REG) {
        auto dest = modrm_decoder->rm_reg();
        registers->set(dest, val);
    } else {
        auto ea = modrm_decoder->effective_address();
        mem->write<T>(ea, val);
    }
}
