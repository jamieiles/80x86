// aas
void EmulatorPimpl::aas3f()
{
    uint16_t flags = registers->get_flags();

    auto al = registers->get(AL);
    auto ah = registers->get(AH);
    if ((al & 0x0f) > 9 || (flags & AF)) {
        al -= 6;
        al &= 0xf;
        --ah;

        flags |= AF | CF;
    } else {
        flags &= ~(AF | CF);
    }

    registers->set(AL, al);
    registers->set(AH, ah);
    registers->set_flags(flags);
}
