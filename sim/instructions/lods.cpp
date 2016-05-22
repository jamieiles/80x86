void EmulatorPimpl::lodsbac()
{
    auto movs = [&]() {
        auto v = mem->read<uint8_t>(get_phys_addr(registers->get(get_segment(false)),
                                                  registers->get(SI)));
        registers->set(AL, v);

        if (registers->get_flag(DF))
            registers->set(SI, registers->get(SI) - 1);
        else
            registers->set(SI, registers->get(SI) + 1);
    };
    do_rep(movs, []() { return false; });
}

void EmulatorPimpl::lodswad()
{
    auto movs = [&]() {
        auto v = mem->read<uint16_t>(get_phys_addr(registers->get(get_segment(false)),
                                                   registers->get(SI)));
        registers->set(AX, v);

        if (registers->get_flag(DF))
            registers->set(SI, registers->get(SI) - 2);
        else
            registers->set(SI, registers->get(SI) + 2);
    };
    do_rep(movs, []() { return false; });
}
