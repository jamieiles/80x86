// in al, data8
void EmulatorPimpl::ine4()
{
    auto port_num = fetch_byte();

    registers->set(AL, read_io8(port_num));
}

// in ax, data8
void EmulatorPimpl::ine5()
{
    auto port_num = fetch_byte();

    registers->set(AX, read_io16(port_num));
}

// in al, dx
void EmulatorPimpl::inec()
{
    registers->set(AL, read_io8(registers->get(DX)));
}

// in ax, dx
void EmulatorPimpl::ined()
{
    registers->set(AX, read_io16(registers->get(DX)));
}

// out data8, al
void EmulatorPimpl::oute6()
{
    auto port_num = fetch_byte();

    write_io8(port_num, registers->get(AL));
}

// out data8, ax
void EmulatorPimpl::oute7()
{
    auto port_num = fetch_byte();

    write_io16(port_num, registers->get(AX));
}

// out dx, al
void EmulatorPimpl::outee()
{
    write_io8(registers->get(DX), registers->get(AL));
}

// out dx, ax
void EmulatorPimpl::outef()
{
    write_io16(registers->get(DX), registers->get(AX));
}
