// aam
void EmulatorPimpl::aamd4()
{
    auto divisor = fetch_byte();

    if (divisor == 0) {
        auto flags = registers->get_flags();
        push_word(flags);
        push_word(registers->get(CS));
        push_word(registers->get(IP));

        flags &= ~(IF | TF);
        registers->set_flags(flags, IF | TF);

        auto new_cs = mem->read<uint16_t>(VEC_DIVIDE_ERROR + 2);
        auto new_ip = mem->read<uint16_t>(VEC_DIVIDE_ERROR + 0);

        registers->set(CS, new_cs);
        registers->set(IP, new_ip);
        jump_taken = true;

        return;
    }

    auto al = registers->get(AL);
    auto quotient = al / divisor;
    auto remainder = al % divisor;

    registers->set(AH, quotient);
    registers->set(AL, remainder);

    uint16_t flags = registers->get_flags();
    flags &= ~(PF | SF | ZF);

    if (!(registers->get(AX) & 0xffff))
        flags |= ZF;
    if (!__builtin_parity(registers->get(AL) & 0xff))
        flags |= PF;
    if (registers->get(AX) & 0x8000)
        flags |= SF;

    registers->set_flags(flags, SF | ZF | PF);
}
