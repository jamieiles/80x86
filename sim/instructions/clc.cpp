void EmulatorPimpl::clcf8()
{
    auto flags = registers->get_flags();

    flags &= ~CF;

    registers->set_flags(flags, CF);
}
