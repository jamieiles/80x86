// inc r/m, 8-bit
void EmulatorPimpl::inc_dec_fe()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() == 7) {
        invalid_opcode();
        return;
    }

    if (modrm_decoder->raw_reg() != 0 && modrm_decoder->raw_reg() != 1) {
        std::cerr << "warning: invalid reg " << std::hex
                  << (unsigned)modrm_decoder->raw_reg() << " for opcode 0x"
                  << opcode << std::endl;
        return;
    }

    uint8_t v = read_data<uint8_t>();
    uint8_t result;
    uint16_t flags;

    if (modrm_decoder->raw_reg() == 0)
        std::tie(flags, result) = do_add<uint8_t>(v, 1);
    else
        std::tie(flags, result) = do_sub<uint8_t>(v, 1);

    registers->set_flags(flags, OF | SF | ZF | AF | PF);
    write_data<uint8_t>(result);
}

// inc r/m, 16-bit
void EmulatorPimpl::incff()
{
    assert(modrm_decoder->raw_reg() == 0);

    uint16_t v = read_data<uint16_t>();
    uint16_t result;
    uint16_t flags;

    std::tie(flags, result) = do_add<uint16_t>(v, 1);

    registers->set_flags(flags, OF | SF | ZF | AF | PF);
    write_data<uint16_t>(result);
}

// inc r, 16-bit
void EmulatorPimpl::inc40_47()
{
    auto reg = static_cast<GPR>(static_cast<int>(AX) + (opcode & 0x7));
    auto v = registers->get(reg);
    uint16_t result;
    uint16_t flags;

    std::tie(flags, result) = do_add<uint16_t>(v, 1);

    registers->set_flags(flags, OF | SF | ZF | AF | PF);
    registers->set(reg, result);
}

// dec r/m, 16-bit
void EmulatorPimpl::decff()
{
    assert(modrm_decoder->raw_reg() == 1);

    uint16_t v = read_data<uint16_t>();
    uint16_t result;
    uint16_t flags;

    std::tie(flags, result) = do_sub<uint16_t>(v, 1);

    registers->set_flags(flags, OF | SF | ZF | AF | PF);
    write_data<uint16_t>(result);
}

// dec r, 16-bit
void EmulatorPimpl::dec48_4f()
{
    auto reg = static_cast<GPR>(static_cast<int>(AX) + (opcode & 0x7));
    auto v = registers->get(reg);
    uint16_t result;
    uint16_t flags;

    std::tie(flags, result) = do_sub<uint16_t>(v, 1);

    registers->set_flags(flags, OF | SF | ZF | AF | PF);
    registers->set(reg, result);
}
