void EmulatorPimpl::cwd99()
{
    int32_t v = sign_extend<int32_t, uint16_t>(registers->get(AX));
    registers->set(DX, (v >> 16) & 0xffff);
}
