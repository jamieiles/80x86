template <typename T>
std::pair<uint16_t, T> do_sar(T v, int count)
{
    uint16_t flags = 0;
    T sign_bit = 0x80 << (8 * (sizeof(T) - 1));
    bool is_negative = !!(v & sign_bit);
    for (int i = 0; i < (count & 0x1f); ++i) {
        flags &= ~CF;
        if (v & 0x1)
            flags |= CF;
        v >>= 1;
        if (is_negative)
            v |= sign_bit;
    }

    if (!v)
        flags |= ZF;
    if (!__builtin_parity(v & 0xff))
        flags |= PF;
    if (v & sign_bit)
        flags |= SF;

    return std::make_pair(flags, v);
}

// sar r/m, 1
void EmulatorPimpl::sard0()
{
    auto v = read_data<uint8_t>();
    uint16_t flags;

    std::tie(flags, v) = do_sar(v, 1);

    write_data<uint8_t>(v);
    registers->set_flags(flags, OF | CF | ZF | PF | SF);
}

// sar r/m, 1
void EmulatorPimpl::sard1()
{
    auto v = read_data<uint16_t>();
    uint16_t flags;

    std::tie(flags, v) = do_sar(v, 1);

    write_data<uint16_t>(v);
    registers->set_flags(flags, OF | CF | ZF | PF | SF);
}

// sar r/m, N
void EmulatorPimpl::sard2()
{
    auto v = read_data<uint8_t>();
    uint16_t flags;

    std::tie(flags, v) = do_sar(v, registers->get(CL));

    write_data<uint8_t>(v);
    registers->set_flags(flags, CF | ZF | PF | SF);
}

// sar r/m, N
void EmulatorPimpl::sard3()
{
    auto v = read_data<uint16_t>();
    uint16_t flags;

    std::tie(flags, v) = do_sar(v, registers->get(CL));

    write_data<uint16_t>(v);
    registers->set_flags(flags, CF | ZF | PF | SF);
}
