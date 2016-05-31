// div r/m, 8-bit
void EmulatorPimpl::divf6()
{
    auto divisor = read_data<uint8_t>();
    auto dividend = registers->get(AX);

    if (divisor == 0 || ((dividend / divisor) & 0xff00)) {
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

    uint8_t quotient = dividend / divisor;
    uint8_t remainder = dividend % divisor;

    registers->set(AH, remainder);
    registers->set(AL, quotient);
}

// div r/m, 16-bit
void EmulatorPimpl::divf7()
{
    auto divisor = read_data<uint16_t>();
    uint32_t dividend =
        (static_cast<uint32_t>(registers->get(DX)) << 16) | registers->get(AX);

    if (divisor == 0 || ((dividend / divisor) & 0xffff0000)) {
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

    uint16_t quotient = dividend / divisor;
    uint16_t remainder = dividend % divisor;

    registers->set(DX, remainder);
    registers->set(AX, quotient);
}

// idiv r/m, 8-bit
void EmulatorPimpl::idivf6()
{
    auto divisor = read_data<uint8_t>();
    auto dividend = registers->get(AX);

    if (divisor == 0 || ((dividend / divisor) & 0xff00)) {
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

    uint8_t quotient = dividend / divisor;
    uint8_t remainder = dividend % divisor;

    registers->set(AH, remainder);
    registers->set(AL, quotient);
}

// div r/m, 16-bit
void EmulatorPimpl::idivf7()
{
    auto divisor = read_data<uint16_t>();
    uint32_t dividend =
        (static_cast<uint32_t>(registers->get(DX)) << 16) | registers->get(AX);

    if (divisor == 0 || ((dividend / divisor) & 0xffff0000)) {
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

    uint16_t quotient = dividend / divisor;
    uint16_t remainder = dividend % divisor;

    registers->set(DX, remainder);
    registers->set(AX, quotient);
}
