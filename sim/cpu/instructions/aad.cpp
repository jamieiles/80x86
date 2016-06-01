// aad
void EmulatorPimpl::aadd5()
{
    uint16_t flags = registers->get_flags();

    uint16_t v = registers->get(AL) + registers->get(AH) * fetch_byte();

    if (v & 0x8000)
        flags |= SF;
    if (!v)
        flags |= ZF;
    if (!__builtin_parity(v & 0xff))
        flags |= PF;

    registers->set(AL, v);
    registers->set(AH, 0);
    registers->set_flags(flags, SF | ZF | PF);
}
