void EmulatorPimpl::outsb6e()
{
    auto outs = [&]() {
        auto v = mem->read<uint8_t>(get_phys_addr(registers->get(DS),
                                                  registers->get(SI)));
        write_io8(registers->get(DX), v);

        if (registers->get_flag(DF))
            registers->set(SI, registers->get(SI) - 1);
        else
            registers->set(SI, registers->get(SI) + 1);
    };
    do_rep(outs, []() { return false; });
}

void EmulatorPimpl::outsw6f()
{
    auto outs = [&]() {
        auto v = mem->read<uint16_t>(get_phys_addr(registers->get(DS),
                                                   registers->get(SI)));
        write_io16(registers->get(DX), v);

        if (registers->get_flag(DF))
            registers->set(SI, registers->get(SI) - 2);
        else
            registers->set(SI, registers->get(SI) + 2);
    };
    do_rep(outs, []() { return false; });
}
