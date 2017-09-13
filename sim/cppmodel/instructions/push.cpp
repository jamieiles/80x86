// push r/m
void EmulatorPimpl::pushff()
{
    assert(modrm_decoder->raw_reg() == 6);

    registers->set(SP, registers->get(SP) - 2);
    auto val = read_data<uint16_t>();
    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    mem->write<uint16_t>(addr, val);
}

// push imm16
void EmulatorPimpl::push68()
{
    auto val = fetch_16bit();

    registers->set(SP, registers->get(SP) - 2);
    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    mem->write<uint16_t>(addr, val);
}

// push imm8
void EmulatorPimpl::push6a()
{
    auto val = sign_extend<int16_t, uint8_t>(fetch_byte());

    registers->set(SP, registers->get(SP) - 2);
    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    mem->write<uint16_t>(addr, val);
}

// push r
void EmulatorPimpl::push50_57()
{
    auto reg = static_cast<GPR>(static_cast<int>(AX) + (opcode & 0x7));

    registers->set(SP, registers->get(SP) - 2);
    auto val = registers->get(reg);
    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    mem->write<uint16_t>(addr, val);
}

// push sr
void EmulatorPimpl::pushsr()
{
    auto reg = static_cast<GPR>(static_cast<int>(ES) + ((opcode >> 3) & 0x3));
    auto val = registers->get(reg);

    registers->set(SP, registers->get(SP) - 2);
    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    mem->write<uint16_t>(addr, val);
}

// pushf
void EmulatorPimpl::pushf9c()
{
    auto val = registers->get_flags();
    registers->set(SP, registers->get(SP) - 2);
    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    mem->write<uint16_t>(addr, val);
}

void EmulatorPimpl::pusha60()
{
    auto sp = registers->get(SP);

    push_word(registers->get(AX));
    push_word(registers->get(CX));
    push_word(registers->get(DX));
    push_word(registers->get(BX));
    push_word(sp);
    push_word(registers->get(BP));
    push_word(registers->get(SI));
    push_word(registers->get(DI));
}

void EmulatorPimpl::popa61()
{
    registers->set(DI, pop_word());
    registers->set(SI, pop_word());
    registers->set(BP, pop_word());
    pop_word();
    registers->set(BX, pop_word());
    registers->set(DX, pop_word());
    registers->set(CX, pop_word());
    registers->set(AX, pop_word());
}
