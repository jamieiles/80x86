void EmulatorPimpl::retc3()
{
    auto ip = pop_word();

    registers->set(IP, ip);
    jump_taken = true;
}

void EmulatorPimpl::retc2()
{
    auto displacement = fetch_16bit();
    auto ip = pop_word();

    registers->set(IP, ip);
    registers->set(SP, registers->get(SP) + displacement);
    jump_taken = true;
}

void EmulatorPimpl::retcb()
{
    auto ip = pop_word();
    auto cs = pop_word();

    registers->set(IP, ip);
    registers->set(CS, cs);
    jump_taken = true;
}

void EmulatorPimpl::retca()
{
    auto displacement = fetch_16bit();
    auto ip = pop_word();
    auto cs = pop_word();

    registers->set(IP, ip);
    registers->set(CS, cs);
    registers->set(SP, registers->get(SP) + displacement);
    jump_taken = true;
}

void EmulatorPimpl::iretcf()
{
    auto ip = pop_word();
    auto cs = pop_word();
    auto flags = pop_word();

    registers->set(IP, ip);
    registers->set(CS, cs);
    registers->set_flags(flags);
    jump_taken = true;
}
