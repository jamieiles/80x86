void EmulatorPimpl::setalcd6()
{
    auto cf = registers->get_flag(CF);

    registers->set(AL, cf ? 0xff : 0x00);
}
