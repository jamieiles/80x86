// xchg r, r/m, 8-bit
void EmulatorPimpl::xchg86()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    auto v1 = read_data<uint8_t>();
    auto v2 = registers->get(modrm_decoder->reg());

    write_data<uint8_t>(v2);
    registers->set(modrm_decoder->reg(), v1);
}

// xchg r, r/m, 16-bit
void EmulatorPimpl::xchg87()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    auto v1 = read_data<uint16_t>();
    auto v2 = registers->get(modrm_decoder->reg());

    write_data<uint16_t>(v2);
    registers->set(modrm_decoder->reg(), v1);
}

// xchg accumulator, r
void EmulatorPimpl::xchg90_97()
{
    auto reg = static_cast<GPR>(static_cast<int>(AX) + (opcode & 0x7));

    auto v1 = registers->get(AX);
    auto v2 = registers->get(reg);

    registers->set(AX, v2);
    registers->set(reg, v1);
}
