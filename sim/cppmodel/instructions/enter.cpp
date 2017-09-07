void EmulatorPimpl::enterc8()
{
    auto size = fetch_16bit();
    auto nest = fetch_byte() & 0x1f;

    push_word(registers->get(BP));
    auto frame_temp = registers->get(SP);

    for (int i = 1; i < nest; ++i) {
        registers->set(BP, registers->get(BP) - 2);
        push_word(mem->read<uint16_t>(
            get_phys_addr(registers->get(SS), registers->get(BP))));
    }

    if (nest)
        push_word(frame_temp);

    registers->set(BP, frame_temp);
    registers->set(SP, registers->get(SP) - size);
}
