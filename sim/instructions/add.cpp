// add r, r/m, 8-bit
void EmulatorPimpl::add00()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = registers->get(modrm_decoder->reg());

    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_add<uint8_t>(v1, v2);

    registers->set_flags(flags);
    write_data<uint8_t>(result & 0xff);
}

// add r, r/m, 16-bit
void EmulatorPimpl::add01()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = registers->get(modrm_decoder->reg());

    uint16_t result, flags;
    std::tie(flags, result) = do_add<uint16_t>(v1, v2);

    registers->set_flags(flags);
    write_data<uint16_t>(result & 0xffff);
}

// add r/m, r, 8-bit
void EmulatorPimpl::add02()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = registers->get(modrm_decoder->reg());

    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_add<uint8_t>(v1, v2);

    registers->set_flags(flags);
    registers->set(modrm_decoder->reg(), result & 0xff);
}

// add r/m, r, 16-bit
void EmulatorPimpl::add03()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = registers->get(modrm_decoder->reg());

    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_add<uint16_t>(v1, v2);

    registers->set_flags(flags);
    registers->set(modrm_decoder->reg(), result & 0xffff);
}

void EmulatorPimpl::add04()
{
    auto v1 = registers->get(AL);
    auto v2 = fetch_byte();
    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_add<uint8_t>(v1, v2);

    registers->set_flags(flags);
    registers->set(AL, result);
}

void EmulatorPimpl::add05()
{
    auto v1 = registers->get(AX);
    auto v2 = fetch_16bit();
    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_add<uint16_t>(v1, v2);

    registers->set_flags(flags);
    registers->set(AX, result);
}
