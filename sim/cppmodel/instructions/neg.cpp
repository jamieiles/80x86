// neg byte r/m
void EmulatorPimpl::negf6()
{
    auto v = read_data<uint8_t>();
    uint16_t flags;
    uint8_t result;
    std::tie(flags, result) = do_sub<uint8_t>(0, v);

    flags &= ~CF;
    if (v != 0)
        flags |= CF;

    write_data<uint8_t>(result);
    registers->set_flags(flags, OF | SF | ZF | CF | PF | AF);
}

// neg word r/m
void EmulatorPimpl::negf7()
{
    auto v = read_data<uint16_t>();
    uint16_t flags;
    uint16_t result;
    std::tie(flags, result) = do_sub<uint16_t>(0, v);

    flags &= ~CF;
    if (v != 0)
        flags |= CF;

    write_data<uint16_t>(result);
    registers->set_flags(flags, OF | SF | ZF | CF | PF | AF);
}
