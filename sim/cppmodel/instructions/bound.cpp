void EmulatorPimpl::bound62()
{
    modrm_decoder->decode();

    if (modrm_decoder->rm_type() == OP_REG) {
        invalid_opcode();
        return;
    }

    auto p32 = get_phys_addr(registers->get(get_segment(false)),
                             modrm_decoder->effective_address());
    auto lower = static_cast<int16_t>(mem->read<uint16_t>(p32));
    auto upper = static_cast<int16_t>(mem->read<uint16_t>(p32 + 2));

    auto v = static_cast<int16_t>(registers->get(modrm_decoder->reg()));

    if (v >= lower && v <= upper)
        return;

    auto flags = registers->get_flags();
    push_word(flags);
    push_word(registers->get(CS));
    push_word(registers->get(IP) + instr_length);

    flags &= ~(IF | TF);
    registers->set_flags(flags, IF | TF);

    // int 3
    auto new_cs = mem->read<uint16_t>(VEC_BOUND + 2);
    auto new_ip = mem->read<uint16_t>(VEC_BOUND + 0);

    registers->set(CS, new_cs);
    registers->set(IP, new_ip);
    jump_taken = true;
}
