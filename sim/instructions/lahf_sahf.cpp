// lahf
void EmulatorPimpl::lahf9f()
{
    auto flags = registers->get_flags();
    registers->set(AH, flags & 0xff);
}

// sahf
void EmulatorPimpl::sahf9e()
{
    auto flags = registers->get(AH);
    registers->set_flags(flags, 0xff);
}
