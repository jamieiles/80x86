// xlat
void EmulatorPimpl::xlatd7()
{
    auto v = registers->get(AL);
    auto table_addr = registers->get(BX);
    auto xlated_val = mem->read<uint8_t>(
        get_phys_addr(registers->get(get_segment(false)), table_addr + v));
    registers->set(AL, xlated_val);
}
