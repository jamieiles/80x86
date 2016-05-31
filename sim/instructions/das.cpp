// das
void EmulatorPimpl::das2f()
{
    uint16_t flags = registers->get_flags();

    auto al = registers->get(AL);
    auto ah = registers->get(AH);
    if ((al & 0x0f) > 9 || (flags & AF)) {
        uint16_t tmp = al - 6;
        al = tmp & 0xff;
        flags |= AF;
        if (tmp & 0xff00)
            flags |= CF;
    } else {
        flags &= ~AF;
    }

    if (al > 0x9f || (flags & CF)) {
        al -= 0x60;
        flags |= CF;
    } else {
        flags &= ~CF;
    }

    if (al & 0x80)
        flags |= SF;
    if (!al)
        flags |= ZF;
    if (!__builtin_parity(al))
        flags |= PF;

    registers->set(AL, al);
    registers->set(AH, ah);
    registers->set_flags(flags, SF | ZF | AF | CF | PF);
}
