void EmulatorPimpl::leavec9()
{
    registers->set(SP, registers->get(BP));
    registers->set(BP, pop_word());
}
