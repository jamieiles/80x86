void EmulatorPimpl::cmpsba6()
{
    auto cmps = [&]() {
        auto s = mem->read<uint8_t>(get_phys_addr(registers->get(get_segment(false)),
                                                  registers->get(SI)));
        auto d = mem->read<uint8_t>(get_phys_addr(registers->get(ES),
                                                  registers->get(DI)));

        uint8_t result;
        uint16_t flags;
        std::tie(flags, result) = do_sub<uint8_t>(s, d);
        registers->set_flags(flags);

        if (registers->get_flag(DF)) {
            registers->set(DI, registers->get(DI) - 1);
            registers->set(SI, registers->get(SI) - 1);
        } else {
            registers->set(DI, registers->get(DI) + 1);
            registers->set(SI, registers->get(SI) + 1);
        }
    };
    do_rep(cmps, [&]() { return string_rep_complete(); });
}
