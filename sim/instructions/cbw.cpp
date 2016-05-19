void EmulatorPimpl::cbw98()
{
    registers->set(AX, sign_extend<int16_t, uint8_t>(registers->get(AL)));
}
