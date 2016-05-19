void EmulatorPimpl::cmcf5()
{
    auto flags = registers->get_flags();

    if (flags & CF)
        flags &= ~CF;
    else
        flags |= CF;

    registers->set_flags(flags);
}
