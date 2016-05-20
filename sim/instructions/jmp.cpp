void EmulatorPimpl::jmpe9()
{
    int16_t displacement = static_cast<int16_t>(fetch_16bit());

    registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jmpea()
{
    uint16_t ip = fetch_16bit();
    uint16_t cs = fetch_16bit();

    registers->set(IP, ip);
    registers->set(CS, cs);
}

void EmulatorPimpl::jmpeb()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::je74()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (registers->get_flags() & ZF)
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jl7c()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (!!(registers->get_flags() & OF) ^ !!(registers->get_flags() & SF))
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jle7e()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if ((!!(registers->get_flags() & OF) ^ !!(registers->get_flags() & SF)) ||
        (registers->get_flags() & ZF))
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jb72()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (registers->get_flags() & CF)
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jbe76()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if ((registers->get_flags() & CF) || (registers->get_flags() & ZF))
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jp7a()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (registers->get_flags() & PF)
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jo70()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (registers->get_flags() & OF)
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::js78()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (registers->get_flags() & SF)
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jne75()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (!(registers->get_flags() & ZF))
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jnl7d()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if ((!!(registers->get_flags() & SF) ^ !!(registers->get_flags() & OF)) == 0)
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jnle7f()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (((registers->get_flag(SF) ^ registers->get_flag(OF)) | registers->get_flag(ZF)) == 0)
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jnb73()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (!registers->get_flag(CF))
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jnbe77()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if ((registers->get_flag(CF) || registers->get_flag(ZF)) == 0)
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jnp7b()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (!registers->get_flag(PF))
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jno71()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (!registers->get_flag(OF))
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jmpff_intra()
{
    auto ip = read_data<uint16_t>();

    registers->set(IP, ip);
}

void EmulatorPimpl::jmpff_inter()
{
    if (modrm_decoder->rm_type() == OP_REG)
        return;

    auto cs_ip = read_data<uint32_t>();

    registers->set(IP, cs_ip & 0xffff);
    registers->set(CS, (cs_ip >> 16) & 0xffff);
}

void EmulatorPimpl::jns79()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (!(registers->get_flags() & SF))
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jcxze3()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (!registers->get(CX))
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}
