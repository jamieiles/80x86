// daa
void EmulatorPimpl::daa27()
{
    uint16_t flags = registers->get_flags();

    auto al = registers->get(AL);
    if ((al & 0x0f) > 9 || (flags & AF)) {
        uint16_t tmp = (al + 6);

        al = tmp & 0xff;
        flags |= AF;
        if (tmp & 0xff00)
            flags |= CF;
    } else {
        flags &= ~AF;
    }

    if (al > 0x9f || (flags & CF)) {
        al += 0x60;
        flags |= CF;
    }

    registers->set(AL, al);
    registers->set_flags(flags, SF | ZF | AF | CF | PF);
}
