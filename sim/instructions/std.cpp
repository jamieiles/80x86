void EmulatorPimpl::stdfd()
{
    auto flags = registers->get_flags();

    flags |= DF;

    registers->set_flags(flags);
}
