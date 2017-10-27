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

// mov m/r, r (8-bit)
void EmulatorPimpl::mov88()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    auto source = modrm_decoder->reg();
    auto val = registers->get(source);

    write_data<uint8_t>(val);
}

// mov m/r, r (16-bit)
void EmulatorPimpl::mov89()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    auto source = modrm_decoder->reg();
    auto val = registers->get(source);

    write_data<uint16_t>(val);
}

// mov r, m/r (8-bit)
void EmulatorPimpl::mov8a()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t val = read_data<uint8_t>();

    auto dest = modrm_decoder->reg();
    registers->set(dest, val);
}

// mov r, m/r (16-bit)
void EmulatorPimpl::mov8b()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t val = read_data<uint16_t>();

    auto dest = modrm_decoder->reg();
    registers->set(dest, val);
}

// mov r/m, immediate (reg == 0), 8-bit
void EmulatorPimpl::movc6()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() == 0) {
        uint8_t immed = fetch_byte();
        write_data<uint8_t>(immed);
    }
}

// mov r/m, immediate (reg == 0), 16-bit
void EmulatorPimpl::movc7()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() == 0)
        write_data<uint16_t>(fetch_16bit());
}

// mov r, immediate, 8-bit
void EmulatorPimpl::movb0_b7()
{
    uint8_t immed = fetch_byte();
    auto reg = static_cast<GPR>(static_cast<int>(AL) + (opcode & 0x7));
    registers->set(reg, immed);
}

// mov r, immediate, 16-bit
void EmulatorPimpl::movb8_bf()
{
    uint16_t immed = fetch_16bit();
    auto reg = static_cast<GPR>(static_cast<int>(AX) + (opcode & 0x7));
    registers->set(reg, immed);
}

// mov al, m, 8-bit
void EmulatorPimpl::mova0()
{
    auto displacement = fetch_16bit();
    auto addr = get_phys_addr(registers->get(get_segment(false)), displacement);
    auto val = mem->read<uint8_t>(addr);
    registers->set(AL, val);
}

// mov ax, m, 16-bit
void EmulatorPimpl::mova1()
{
    auto displacement = fetch_16bit();
    auto addr = get_phys_addr(registers->get(get_segment(false)), displacement);
    auto val = mem->read<uint16_t>(addr);
    registers->set(AX, val);
}

// mov m, al 8-bit
void EmulatorPimpl::mova2()
{
    auto displacement = fetch_16bit();
    auto val = registers->get(AL);
    auto addr = get_phys_addr(registers->get(get_segment(false)), displacement);
    mem->write<uint8_t>(addr, val);
}

// mov m, al 16-bit
void EmulatorPimpl::mova3()
{
    auto displacement = fetch_16bit();
    auto val = registers->get(AX);
    auto addr = get_phys_addr(registers->get(get_segment(false)), displacement);
    mem->write<uint16_t>(addr, val);
}

// mov sr, r/m
void EmulatorPimpl::mov8e()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t val = read_data<uint16_t>();
    auto segnum = modrm_decoder->raw_reg() & 0x3;
    auto reg = static_cast<GPR>(static_cast<int>(ES) + segnum);

    registers->set(reg, val);
    ext_int_inhibit = true;
}

// mov r/m, sr
void EmulatorPimpl::mov8c()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    auto segnum = modrm_decoder->raw_reg() & 0x3;
    auto reg = static_cast<GPR>(static_cast<int>(ES) + segnum);
    uint16_t val = registers->get(reg);

    write_data<uint16_t>(val);
}
