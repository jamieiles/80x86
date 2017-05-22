template <typename T>
std::pair<uint16_t, T> do_rcr(T v, int count, bool carry_in)
{
    uint16_t flags = carry_in ? CF : 0;

    for (int i = 0; i < (count & 0x1f); ++i) {
        bool carry_bit_set = !!(flags & CF);
        flags &= ~CF;
        T lsb = v & 1;
        v >>= 1;
        v += static_cast<T>(carry_bit_set) << ((sizeof(T) * 8) - 1);

        if (lsb)
            flags |= CF;
    }

    // 2 MSB's XOR'd == OF
    T msbs = (v >> ((sizeof(T) * 8) - 2)) & 0x3;
    if (msbs == 2 || msbs == 1)
        flags |= OF;

    return std::make_pair(flags, v);
}

// rcr r/m, N
void EmulatorPimpl::rcrc0()
{
    auto v = read_data<uint8_t>();
    auto count = fetch_byte();
    uint16_t flags;

    if (!(count & 0x1f))
        return;

    std::tie(flags, v) = do_rcr(v, count & 0x1f, registers->get_flag(CF));

    write_data<uint8_t>(v);
    registers->set_flags(flags, CF);
}

// rcr r/m, 1
void EmulatorPimpl::rcrc1()
{
    auto v = read_data<uint16_t>();
    auto count = fetch_byte();
    uint16_t flags;

    if (!(count & 0x1f))
        return;

    std::tie(flags, v) = do_rcr(v, count & 0x1f, registers->get_flag(CF));

    write_data<uint16_t>(v);
    registers->set_flags(flags, CF);
}

// rcr r/m, 1
void EmulatorPimpl::rcrd0()
{
    auto v = read_data<uint8_t>();
    uint16_t flags;

    std::tie(flags, v) = do_rcr(v, 1, registers->get_flag(CF));

    write_data<uint8_t>(v);
    registers->set_flags(flags, CF | OF);
}

// rcr r/m, 1
void EmulatorPimpl::rcrd1()
{
    auto v = read_data<uint16_t>();
    uint16_t flags;

    std::tie(flags, v) = do_rcr(v, 1, registers->get_flag(CF));

    write_data<uint16_t>(v);
    registers->set_flags(flags, CF | OF);
}

// rcr r/m, N
void EmulatorPimpl::rcrd2()
{
    auto v = read_data<uint8_t>();
    uint16_t flags;

    if (!registers->get(CL))
        return;

    std::tie(flags, v) = do_rcr(v, registers->get(CL),
                                registers->get_flag(CF));

    write_data<uint8_t>(v);
    registers->set_flags(flags, CF);
}

// rcr r/m, N
void EmulatorPimpl::rcrd3()
{
    auto v = read_data<uint16_t>();
    uint16_t flags;

    if (!registers->get(CL))
        return;

    std::tie(flags, v) = do_rcr(v, registers->get(CL),
                                registers->get_flag(CF));

    write_data<uint16_t>(v);
    registers->set_flags(flags, CF);
}
