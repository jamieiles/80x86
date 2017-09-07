void EmulatorPimpl::movsba4()
{
    auto movs = [&]() {
        auto v = mem->read<uint8_t>(get_phys_addr(
            registers->get(get_segment(false)), registers->get(SI)));
        mem->write<uint8_t>(
            get_phys_addr(registers->get(ES), registers->get(DI)), v);

        if (registers->get_flag(DF)) {
            registers->set(DI, registers->get(DI) - 1);
            registers->set(SI, registers->get(SI) - 1);
        } else {
            registers->set(DI, registers->get(DI) + 1);
            registers->set(SI, registers->get(SI) + 1);
        }
    };
    do_rep(movs, []() { return false; });
}

void EmulatorPimpl::movswa5()
{
    auto movs = [&]() {
        auto v = mem->read<uint16_t>(get_phys_addr(
            registers->get(get_segment(false)), registers->get(SI)));
        mem->write<uint16_t>(
            get_phys_addr(registers->get(ES), registers->get(DI)), v);

        if (registers->get_flag(DF)) {
            registers->set(DI, registers->get(DI) - 2);
            registers->set(SI, registers->get(SI) - 2);
        } else {
            registers->set(DI, registers->get(DI) + 2);
            registers->set(SI, registers->get(SI) + 2);
        }
    };
    do_rep(movs, []() { return false; });
}
