void EmulatorPimpl::stosbaa()
{
    auto stos = [&]() {
        auto v = registers->get(AL);
        mem->write<uint8_t>(get_phys_addr(registers->get(ES),
                                          registers->get(DI)), v);
        registers->set(AL, v);

        if (registers->get_flag(DF))
            registers->set(DI, registers->get(DI) - 1);
        else
            registers->set(DI, registers->get(DI) + 1);
    };
    do_rep(stos, []() { return false; });
}

void EmulatorPimpl::stoswab()
{
    auto stos = [&]() {
        auto v = registers->get(AX);
        mem->write<uint16_t>(get_phys_addr(registers->get(ES),
                                           registers->get(DI)), v);

        if (registers->get_flag(DF))
            registers->set(DI, registers->get(DI) - 2);
        else
            registers->set(DI, registers->get(DI) + 2);
    };
    do_rep(stos, []() { return false; });
}
