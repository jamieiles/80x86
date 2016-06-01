void EmulatorPimpl::loopnze0()
{
    int16_t displacement = static_cast<int8_t>(fetch_byte());

    registers->set(CX, registers->get(CX) - 1);

    if (registers->get(CX) != 0 && !registers->get_flag(ZF)) {
        registers->set(IP, registers->get(IP) + instr_length + displacement);
        jump_taken = true;
    }
}
