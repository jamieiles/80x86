void EmulatorPimpl::calle8()
{
    auto displacement = fetch_16bit();

    push_word(registers->get(IP) + instr_length);

    registers->set(IP, registers->get(IP) + instr_length + displacement);
}

void EmulatorPimpl::callff_intra()
{
    auto displacement = read_data<uint16_t>();

    push_word(registers->get(IP) + instr_length);

    registers->set(IP, displacement);
}

void EmulatorPimpl::callff_inter()
{
    if (modrm_decoder->rm_type() == OP_REG)
        return;

    push_word(registers->get(CS));
    push_word(registers->get(IP) + instr_length);

    auto cs_ip = read_data<uint32_t>();

    registers->set(IP, cs_ip & 0xffff);
    registers->set(CS, (cs_ip >> 16) & 0xffff);
}

void EmulatorPimpl::call9a()
{
    auto new_ip = fetch_16bit();
    auto new_cs = fetch_16bit();

    push_word(registers->get(CS));
    push_word(registers->get(IP) + instr_length);

    registers->set(CS, new_cs);
    registers->set(IP, new_ip);
}
