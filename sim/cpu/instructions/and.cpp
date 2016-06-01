// and r, r/m, 8-bit
void EmulatorPimpl::and20()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = registers->get(modrm_decoder->reg());

    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_and<uint8_t>(v1, v2);
    flags &= ~(CF | OF);

    registers->set_flags(flags, OF | SF | ZF | CF | PF);
    write_data<uint8_t>(result & 0xff);
}

// and r, r/m, 16-bit
void EmulatorPimpl::and21()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = registers->get(modrm_decoder->reg());

    uint16_t result, flags;
    std::tie(flags, result) = do_and<uint16_t>(v1, v2);
    flags &= ~(CF | OF);

    registers->set_flags(flags, OF | SF | ZF | CF | PF);
    write_data<uint16_t>(result & 0xffff);
}

// and r/m, r, 8-bit
void EmulatorPimpl::and22()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = registers->get(modrm_decoder->reg());

    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_and<uint8_t>(v1, v2);
    flags &= ~(CF | OF);

    registers->set_flags(flags, OF | SF | ZF | CF | PF);
    registers->set(modrm_decoder->reg(), result & 0xff);
}

// and r/m, r, 16-bit
void EmulatorPimpl::and23()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = registers->get(modrm_decoder->reg());

    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_and<uint16_t>(v1, v2);
    flags &= ~(CF | OF);

    registers->set_flags(flags, OF | SF | ZF | CF | PF);
    registers->set(modrm_decoder->reg(), result & 0xffff);
}

void EmulatorPimpl::and24()
{
    auto v1 = registers->get(AL);
    auto v2 = fetch_byte();
    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_and<uint8_t>(v1, v2);
    flags &= ~(CF | OF);

    registers->set_flags(flags, OF | SF | ZF | CF | PF);
    registers->set(AL, result);
}

void EmulatorPimpl::and25()
{
    auto v1 = registers->get(AX);
    auto v2 = fetch_16bit();
    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_and<uint16_t>(v1, v2);
    flags &= ~(CF | OF);

    registers->set_flags(flags, OF | SF | ZF | CF | PF);
    registers->set(AX, result);
}
