template <typename T>
std::pair<uint16_t, T> do_shl(T v, int count)
{
    uint16_t flags = 0;
    T overflow_test_bits = 0xc0 << (8 * (sizeof(T) - 1));
    if ((v & overflow_test_bits) == overflow_test_bits)
        flags |= OF;

    T sign_bit = 0x80 << (8 * (sizeof(T) - 1));
    for (int i = 0; i < (count & 0x1f); ++i) {
        if (v & sign_bit)
            flags |= CF;
        v <<= 1;
    }

    if (!v)
        flags |= ZF;
    if (!__builtin_parity(v & 0xff))
        flags |= PF;
    if (v & sign_bit)
        flags |= SF;

    return std::make_pair(flags, v);
}

// shl r/m, 1
void EmulatorPimpl::shld0()
{
    auto v = read_data<uint8_t>();
    uint16_t flags;

    std::tie(flags, v) = do_shl(v, 1);

    write_data<uint8_t>(v);
    registers->set_flags(flags, OF | CF | ZF | PF | SF);
}

// shl r/m, 1
void EmulatorPimpl::shld1()
{
    auto v = read_data<uint16_t>();
    uint16_t flags;

    std::tie(flags, v) = do_shl(v, 1);

    write_data<uint16_t>(v);
    registers->set_flags(flags, OF | CF | ZF | PF | SF);
}

// shl r/m, N
void EmulatorPimpl::shld2()
{
    auto v = read_data<uint8_t>();
    uint16_t flags;

    std::tie(flags, v) = do_shl(v, registers->get(CL));

    write_data<uint8_t>(v);
    registers->set_flags(flags, CF | ZF | PF | SF);
}

// shl r/m, N
void EmulatorPimpl::shld3()
{
    auto v = read_data<uint16_t>();
    uint16_t flags;

    std::tie(flags, v) = do_shl(v, registers->get(CL));

    write_data<uint16_t>(v);
    registers->set_flags(flags, CF | ZF | PF | SF);
}
