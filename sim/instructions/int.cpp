void EmulatorPimpl::intcc()
{
    auto flags = registers->get_flags();

    push_word(flags);
    push_word(registers->get(CS));
    push_word(registers->get(IP) + instr_length);

    flags &= ~(IF | TF);
    registers->set_flags(flags, IF | TF);

    // int 3
    auto new_cs = mem->read<uint16_t>(VEC_INT + 2);
    auto new_ip = mem->read<uint16_t>(VEC_INT + 0);

    registers->set(CS, new_cs);
    registers->set(IP, new_ip);
}

void EmulatorPimpl::intcd()
{
    auto flags = registers->get_flags();
    auto int_num = fetch_byte();

    push_word(flags);
    push_word(registers->get(CS));
    push_word(registers->get(IP) + instr_length);

    flags &= ~(IF | TF);
    registers->set_flags(flags, IF | TF);

    // int 3
    auto new_cs = mem->read<uint16_t>(int_num * 4 + 2);
    auto new_ip = mem->read<uint16_t>(int_num * 4 + 0);

    registers->set(CS, new_cs);
    registers->set(IP, new_ip);
}

void EmulatorPimpl::intoce()
{
    auto flags = registers->get_flags();
    if (!(flags & OF))
        return;

    push_word(flags);
    push_word(registers->get(CS));
    push_word(registers->get(IP) + instr_length);

    flags &= ~(IF | TF);
    registers->set_flags(flags, IF | TF);

    // int 3
    auto new_cs = mem->read<uint16_t>(VEC_OVERFLOW + 2);
    auto new_ip = mem->read<uint16_t>(VEC_OVERFLOW + 0);

    registers->set(CS, new_cs);
    registers->set(IP, new_ip);
}
