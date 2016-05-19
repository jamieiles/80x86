// imul r/m, 8-bit
void EmulatorPimpl::imulf6()
{
    auto old_flags = registers->get_flags();
    int16_t v1 = sign_extend<int16_t, uint8_t>(read_data<uint8_t>());
    int16_t v2 = sign_extend<int16_t, uint8_t>(registers->get(AL));

    uint16_t result, flags;
    std::tie(flags, result) = do_mul<int16_t>(v1, v2);
    flags = old_flags & ~(CF | OF);
    if ((result & 0xff80) != 0xff80 &&
        (result & 0xff80) != 0x0000)
        flags |= CF | OF;

    registers->set(AX, result);
    registers->set_flags(flags);
}

// imul r/m, 16-bit
void EmulatorPimpl::imulf7()
{
    auto old_flags = registers->get_flags();
    int32_t v1 = sign_extend<int32_t, uint16_t>(read_data<uint16_t>());
    int32_t v2 = sign_extend<int32_t, uint16_t>(registers->get(AX));

    uint32_t result;
    uint16_t flags;
    std::tie(flags, result) = do_mul<int32_t>(v1, v2);
    flags = old_flags & ~(CF | OF);
    if ((result & 0xffff8000) != 0xffff8000 &&
        (result & 0xffff8000) != 0x00000000)
        flags |= CF | OF;

    registers->set(AX, result & 0xffff);
    registers->set(DX, (result >> 16) & 0xffff);
    registers->set_flags(flags);
}
