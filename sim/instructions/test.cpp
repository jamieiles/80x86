// test r, r/m, 8-bit
void EmulatorPimpl::test84()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = registers->get(modrm_decoder->reg());

    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_and<uint8_t>(v1, v2);

    registers->set_flags(flags, OF | SF | ZF | AF | CF | PF);
}

// test r, r/m, 16-bit
void EmulatorPimpl::test85()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = registers->get(modrm_decoder->reg());

    uint16_t result, flags;
    std::tie(flags, result) = do_and<uint16_t>(v1, v2);

    registers->set_flags(flags, OF | SF | ZF | AF | CF | PF);
}

void EmulatorPimpl::testa8()
{
    auto v1 = registers->get(AL);
    auto v2 = fetch_byte();
    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_and<uint8_t>(v1, v2);

    registers->set_flags(flags, OF | SF | ZF | AF | CF | PF);
}

void EmulatorPimpl::testa9()
{
    auto v1 = registers->get(AX);
    auto v2 = fetch_16bit();
    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_and<uint16_t>(v1, v2);

    registers->set_flags(flags, OF | SF | ZF | AF | CF | PF);
}

// test byte r/m
void EmulatorPimpl::testf6()
{
    auto v1 = read_data<uint8_t>();
    auto v2 = fetch_byte();
    uint16_t flags;
    uint8_t result;
    std::tie(flags, result) = do_and<uint8_t>(v1, v2);

    registers->set_flags(flags, OF | SF | ZF | CF | PF);
}

// test word r/m
void EmulatorPimpl::testf7()
{
    auto v1 = read_data<uint16_t>();
    auto v2 = fetch_16bit();
    uint16_t flags;
    uint16_t result;
    std::tie(flags, result) = do_and<uint16_t>(v1, v2);

    registers->set_flags(flags, OF | SF | ZF | CF | PF);
}
