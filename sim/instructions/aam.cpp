// aam
void EmulatorPimpl::aamd4()
{
    auto divisor = fetch_byte();

    auto al = registers->get(AL);
    auto quotient = al / divisor;
    auto remainder = al % divisor;

    registers->set(AH, quotient);
    registers->set(AL, remainder);

    uint16_t flags = registers->get_flags();
    flags &= ~(PF | SF | ZF);

    if (!(registers->get(AL) & 0xff))
        flags |= ZF;
    if (!__builtin_parity(registers->get(AL) & 0xff))
        flags |= PF;
    if (registers->get(AL) & 0x80)
        flags |= SF;

    registers->set_flags(flags);
}

