// lea r, r/m
void EmulatorPimpl::lea8d()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    if (modrm_decoder->rm_type() != OP_REG)
        registers->set(modrm_decoder->reg(),
                       modrm_decoder->effective_address());
}

// lds r, m
void EmulatorPimpl::ldsc5()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    if (modrm_decoder->rm_type() == OP_REG)
        return;

    auto p32 = get_phys_addr(registers->get(get_segment(false)),
                             modrm_decoder->effective_address());
    auto displacement = mem->read<uint16_t>(p32);
    auto seg = mem->read<uint16_t>(p32 + 2);

    registers->set(modrm_decoder->reg(), displacement);
    registers->set(DS, seg);
}

// les r, m
void EmulatorPimpl::lesc4()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    if (modrm_decoder->rm_type() == OP_REG)
        return;

    auto p32 = get_phys_addr(registers->get(get_segment(false)),
                             modrm_decoder->effective_address());
    auto displacement = mem->read<uint16_t>(p32);
    auto seg = mem->read<uint16_t>(p32 + 2);

    registers->set(modrm_decoder->reg(), displacement);
    registers->set(ES, seg);
}
