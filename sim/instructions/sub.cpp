// sub r, r/m, 8-bit
void EmulatorPimpl::sub28()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = registers->get(modrm_decoder->reg());

    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint8_t>(v1, v2);

    registers->set_flags(flags, OF | SF | ZF | CF | PF | AF);
    write_data<uint8_t>(result & 0xff);
}

// sub r, r/m, 16-bit
void EmulatorPimpl::sub29()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = registers->get(modrm_decoder->reg());

    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint16_t>(v1, v2);

    registers->set_flags(flags, OF | SF | ZF | CF | PF | AF);
    write_data<uint16_t>(result & 0xffff);
}

// sub r/m, r, 8-bit
void EmulatorPimpl::sub2a()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = registers->get(modrm_decoder->reg());

    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint8_t>(v2, v1);

    registers->set_flags(flags, OF | SF | ZF | CF | PF | AF);
    registers->set(modrm_decoder->reg(), result & 0xff);
}

// sub r/m, r, 16-bit
void EmulatorPimpl::sub2b()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = registers->get(modrm_decoder->reg());

    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint16_t>(v2, v1);

    registers->set_flags(flags, OF | SF | ZF | CF | PF | AF);
    registers->set(modrm_decoder->reg(), result & 0xffff);
}

void EmulatorPimpl::sub2c()
{
    auto v1 = registers->get(AL);
    auto v2 = fetch_byte();
    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint8_t>(v1, v2);

    registers->set_flags(flags, OF | SF | ZF | CF | PF | AF);
    registers->set(AL, result);
}

void EmulatorPimpl::sub2d()
{
    auto v1 = registers->get(AX);
    auto v2 = fetch_16bit();
    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint16_t>(v1, v2);

    registers->set_flags(flags, OF | SF | ZF | CF | PF | AF);
    registers->set(AX, result);
}
