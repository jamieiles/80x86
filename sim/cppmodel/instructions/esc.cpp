void EmulatorPimpl::escd8()
{
    modrm_decoder->decode();

    auto flags = registers->get_flags();

    push_word(flags);
    push_word(registers->get(CS));
    push_word(registers->get(IP));

    flags &= ~(IF | TF);
    registers->set_flags(flags, IF | TF);

    auto new_cs = mem->read<uint16_t>(VEC_ESCAPE + 2);
    auto new_ip = mem->read<uint16_t>(VEC_ESCAPE + 0);

    registers->set(CS, new_cs);
    registers->set(IP, new_ip);
    jump_taken = true;
}
