// in al, data8
void EmulatorPimpl::ine4()
{
    auto port_num = fetch_byte();

    registers->set(AL, io->read<uint8_t>(port_num));
}

// in ax, data8
void EmulatorPimpl::ine5()
{
    auto port_num = fetch_byte();

    registers->set(AX, io->read<uint16_t>(port_num));
}

// in al, dx
void EmulatorPimpl::inec()
{
    registers->set(AL, io->read<uint8_t>(registers->get(DX)));
}

// in ax, dx
void EmulatorPimpl::ined()
{
    registers->set(AX, io->read<uint16_t>(registers->get(DX)));
}

// out data8, al
void EmulatorPimpl::oute6()
{
    auto port_num = fetch_byte();

    io->write<uint8_t>(port_num, registers->get(AL));
}

// out data8, ax
void EmulatorPimpl::oute7()
{
    auto port_num = fetch_byte();

    io->write<uint16_t>(port_num, registers->get(AX));
}

// out dx, al
void EmulatorPimpl::outee()
{
    io->write<uint8_t>(registers->get(DX), registers->get(AL));
}

// out dx, ax
void EmulatorPimpl::outef()
{
    io->write<uint16_t>(registers->get(DX), registers->get(AX));
}
