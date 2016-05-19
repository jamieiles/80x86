// pop r/m
void EmulatorPimpl::pop8f()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() != 0)
        return;

    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    auto val = mem->read<uint16_t>(addr);
    registers->set(SP, registers->get(SP) + 2);

    write_data<uint16_t>(val);
}

// pop r
void EmulatorPimpl::pop58_5f()
{
    auto reg = static_cast<GPR>(static_cast<int>(AX) + (opcode & 0x7));
    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    auto val = mem->read<uint16_t>(addr);

    registers->set(SP, registers->get(SP) + 2);
    registers->set(reg, val);
}

// pop sr
void EmulatorPimpl::popsr()
{
    auto reg = static_cast<GPR>(static_cast<int>(ES) + ((opcode >> 3) & 0x3));
    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    auto val = mem->read<uint16_t>(addr);

    registers->set(reg, val);
    registers->set(SP, registers->get(SP) + 2);
}

// popf
void EmulatorPimpl::popf9d()
{
    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    auto val = mem->read<uint16_t>(addr);
    registers->set_flags(val);

    registers->set(SP, registers->get(SP) + 2);
}
