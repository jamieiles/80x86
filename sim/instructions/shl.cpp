// shl r/m, 1
void EmulatorPimpl::shld0()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    auto v = read_data<uint8_t>();

    uint16_t flags = 0;
    if ((v & 0xc0) == 0xc0)
        flags |= OF;
    if (v & 0x80)
        flags |= CF;

    v <<= 1;

    if (!v)
        flags |= ZF;
    if (!__builtin_parity(v))
        flags |= PF;
    if (v & 0x80)
        flags |= SF;

    write_data<uint8_t>(v);
    registers->set_flags(flags, OF | CF | ZF | PF | SF);
}

// shl r/m, 1
void EmulatorPimpl::shld1()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    auto v = read_data<uint16_t>();

    uint16_t flags = 0;
    if ((v & 0xc000) == 0xc000)
        flags |= OF;
    if (v & 0x8000)
        flags |= CF;

    v <<= 1;

    if (!v)
        flags |= ZF;
    if (!__builtin_parity(v & 0xff))
        flags |= PF;
    if (v & 0x8000)
        flags |= SF;

    write_data<uint16_t>(v);
    registers->set_flags(flags, OF | CF | ZF | PF | SF);
}
