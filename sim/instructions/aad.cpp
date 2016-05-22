// aad
void EmulatorPimpl::aadd5()
{
    uint16_t flags = registers->get_flags();

    auto al = registers->get(AL) + registers->get(AH) * fetch_byte();

    if (al & 0x80)
        flags |= SF;
    if (!al)
        flags |= ZF;
    if (!__builtin_parity(al))
        flags |= PF;

    registers->set(AL, al);
    registers->set(AH, 0);
    registers->set_flags(flags, SF | ZF | PF);
}
