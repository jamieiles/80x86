// lahf
void EmulatorPimpl::lahf9f()
{
    auto flags = registers->get_flags();
    registers->set(AH, flags & 0xff);
}

// sahf
void EmulatorPimpl::sahf9e()
{
    auto new_flags = registers->get(AH);
    auto old_flags = registers->get_flags();
    registers->set_flags((old_flags & 0xff00) | new_flags);
}
