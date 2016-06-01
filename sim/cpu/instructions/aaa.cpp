// aaa
void EmulatorPimpl::aaa37()
{
    uint16_t flags = registers->get_flags();

    auto al = registers->get(AL);
    if ((al & 0x0f) > 9 || (flags & AF)) {
        al = (al + 6) & 0x0f;
        registers->set(AL, al);

        auto ah = registers->get(AH);
        ah++;
        registers->set(AH, ah);

        flags |= AF | CF;
    } else {
        flags &= ~(AF | CF);
    }

    registers->set_flags(flags, AF | CF);
}
