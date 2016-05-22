void EmulatorPimpl::scasbae()
{
    auto scas = [&]() {
        auto v = mem->read<uint8_t>(get_phys_addr(registers->get(ES),
                                                  registers->get(DI)));

        uint8_t result;
        uint16_t flags;
        std::tie(flags, result) = do_sub<uint8_t>(v, registers->get(AL));
        registers->set_flags(flags, OF | SF | ZF | CF | PF | AF);

        if (registers->get_flag(DF))
            registers->set(DI, registers->get(DI) - 1);
        else
            registers->set(DI, registers->get(DI) + 1);
    };
    do_rep(scas, [&]() { return string_rep_complete(); });
}

void EmulatorPimpl::scasbaf()
{
    auto scas = [&]() {
        auto v = mem->read<uint16_t>(get_phys_addr(registers->get(ES),
                                                   registers->get(DI)));

        uint16_t result;
        uint16_t flags;
        std::tie(flags, result) = do_sub<uint16_t>(v, registers->get(AX));
        registers->set_flags(flags, OF | SF | ZF | CF | PF | AF);

        if (registers->get_flag(DF))
            registers->set(DI, registers->get(DI) - 2);
        else
            registers->set(DI, registers->get(DI) + 2);
    };
    do_rep(scas, [&]() { return string_rep_complete(); });
}
