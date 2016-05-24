// xor r, r/m, 8-bit
void EmulatorPimpl::xor30()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = registers->get(modrm_decoder->reg());

    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_xor<uint8_t>(v1, v2);

    registers->set_flags(flags, OF | SF | ZF | AF | CF | PF);
    write_data<uint8_t>(result & 0xff);
}

// xor r, r/m, 16-bit
void EmulatorPimpl::xor31()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = registers->get(modrm_decoder->reg());

    uint16_t result, flags;
    std::tie(flags, result) = do_xor<uint16_t>(v1, v2);

    registers->set_flags(flags, OF | SF | ZF | AF | CF | PF);
    write_data<uint16_t>(result & 0xffff);
}

// xor r/m, r, 8-bit
void EmulatorPimpl::xor32()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = registers->get(modrm_decoder->reg());

    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_xor<uint8_t>(v1, v2);

    registers->set_flags(flags, OF | SF | ZF | AF | CF | PF);
    registers->set(modrm_decoder->reg(), result & 0xff);
}

// xor r/m, r, 16-bit
void EmulatorPimpl::xor33()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = registers->get(modrm_decoder->reg());

    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_xor<uint16_t>(v1, v2);

    registers->set_flags(flags, OF | SF | ZF | AF | CF | PF);
    registers->set(modrm_decoder->reg(), result & 0xffff);
}

void EmulatorPimpl::xor34()
{
    auto v1 = registers->get(AL);
    auto v2 = fetch_byte();
    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_xor<uint8_t>(v1, v2);

    registers->set_flags(flags, OF | SF | ZF | AF | CF | PF);
    registers->set(AL, result);
}

void EmulatorPimpl::xor35()
{
    auto v1 = registers->get(AX);
    auto v2 = fetch_16bit();
    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_xor<uint16_t>(v1, v2);

    registers->set_flags(flags, OF | SF | ZF | AF | CF | PF);
    registers->set(AX, result);
}
