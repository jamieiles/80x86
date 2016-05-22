// not byte r/m
void EmulatorPimpl::notf6()
{
    auto v = read_data<uint8_t>();
    write_data<uint8_t>(~v);
}

// not word r/m
void EmulatorPimpl::notf7()
{
    auto v = read_data<uint16_t>();
    write_data<uint16_t>(~v);
}
