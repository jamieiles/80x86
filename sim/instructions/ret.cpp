void EmulatorPimpl::retc3()
{
    auto ip = pop_word();

    registers->set(IP, ip);
}

void EmulatorPimpl::retc2()
{
    auto displacement = fetch_16bit();
    auto ip = pop_word();

    registers->set(IP, ip);
    registers->set(SP, registers->get(SP) + displacement);
}

void EmulatorPimpl::retcb()
{
    auto ip = pop_word();
    auto cs = pop_word();

    registers->set(IP, ip);
    registers->set(CS, cs);
}

void EmulatorPimpl::retca()
{
    auto displacement = fetch_16bit();
    auto ip = pop_word();
    auto cs = pop_word();

    registers->set(IP, ip);
    registers->set(CS, cs);
    registers->set(SP, registers->get(SP) + displacement);
}

void EmulatorPimpl::iretcf()
{
    auto ip = pop_word();
    auto cs = pop_word();
    auto flags = pop_word();

    registers->set(IP, ip);
    registers->set(CS, cs);
    registers->set_flags(flags);
}
