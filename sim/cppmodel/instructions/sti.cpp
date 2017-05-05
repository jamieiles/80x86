void EmulatorPimpl::stifb()
{
    auto flags = registers->get_flags();

    flags |= IF;

    registers->set_flags(flags, IF);
    ext_int_inhibit = true;
}
