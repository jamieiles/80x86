void EmulatorPimpl::insb6c()
{
    auto ins = [&]() {
        auto v = read_io8(registers->get(DX));
        mem->write<uint8_t>(
            get_phys_addr(registers->get(ES), registers->get(DI)), v);

        if (registers->get_flag(DF))
            registers->set(DI, registers->get(DI) - 1);
        else
            registers->set(DI, registers->get(DI) + 1);
    };
    do_rep(ins, []() { return false; });
}

void EmulatorPimpl::insw6d()
{
    auto ins = [&]() {
        auto v = read_io16(registers->get(DX));
        mem->write<uint16_t>(
            get_phys_addr(registers->get(ES), registers->get(DI)), v);

        if (registers->get_flag(DF))
            registers->set(DI, registers->get(DI) - 2);
        else
            registers->set(DI, registers->get(DI) + 2);
    };
    do_rep(ins, []() { return false; });
}
