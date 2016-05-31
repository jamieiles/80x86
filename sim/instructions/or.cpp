// or r, r/m, 8-bit
void EmulatorPimpl::or08()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = registers->get(modrm_decoder->reg());

    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_or<uint8_t>(v1, v2);
    flags &= ~(CF | OF);

    registers->set_flags(flags, OF | SF | ZF | CF | PF);
    write_data<uint8_t>(result & 0xff);
}

// or r, r/m, 16-bit
void EmulatorPimpl::or09()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = registers->get(modrm_decoder->reg());

    uint16_t result, flags;
    std::tie(flags, result) = do_or<uint16_t>(v1, v2);
    flags &= ~(CF | OF);

    registers->set_flags(flags, OF | SF | ZF | CF | PF);
    write_data<uint16_t>(result & 0xffff);
}

// or r/m, r, 8-bit
void EmulatorPimpl::or0a()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = registers->get(modrm_decoder->reg());

    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_or<uint8_t>(v1, v2);
    flags &= ~(CF | OF);

    registers->set_flags(flags, OF | SF | ZF | CF | PF);
    registers->set(modrm_decoder->reg(), result & 0xff);
}

// or r/m, r, 16-bit
void EmulatorPimpl::or0b()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = registers->get(modrm_decoder->reg());

    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_or<uint16_t>(v1, v2);
    flags &= ~(CF | OF);

    registers->set_flags(flags, OF | SF | ZF | CF | PF);
    registers->set(modrm_decoder->reg(), result & 0xffff);
}

void EmulatorPimpl::or0c()
{
    auto v1 = registers->get(AL);
    auto v2 = fetch_byte();
    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_or<uint8_t>(v1, v2);
    flags &= ~(CF | OF);

    registers->set_flags(flags, OF | SF | ZF | CF | PF);
    registers->set(AL, result);
}

void EmulatorPimpl::or0d()
{
    auto v1 = registers->get(AX);
    auto v2 = fetch_16bit();
    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_or<uint16_t>(v1, v2);
    flags &= ~(CF | OF);

    registers->set_flags(flags, OF | SF | ZF | CF | PF);
    registers->set(AX, result);
}
