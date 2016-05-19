// mul r/m, 8-bit
void EmulatorPimpl::mulf6()
{
    auto old_flags = registers->get_flags();
    auto v1 = read_data<uint8_t>();
    auto v2 = registers->get(AL);

    uint16_t result, flags;
    std::tie(flags, result) = do_mul<uint16_t>(v1, v2);

    flags = old_flags & ~(CF | OF);
    if (result & 0xff00)
        flags |= CF | OF;

    registers->set(AX, result);
    registers->set_flags(flags);
}

// mul r/m, 16-bit
void EmulatorPimpl::mulf7()
{
    auto old_flags = registers->get_flags();
    auto v1 = read_data<uint16_t>();
    auto v2 = registers->get(AX);

    uint32_t result;
    uint16_t flags;
    std::tie(flags, result) = do_mul<uint32_t>(v1, v2);

    flags = old_flags & ~(CF | OF);
    if (result & 0xffff0000)
        flags |= CF | OF;

    registers->set(AX, result & 0xffff);
    registers->set(DX, (result >> 16) & 0xffff);
    registers->set_flags(flags);
}
