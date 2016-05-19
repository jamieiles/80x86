#include "Emulate.h"

#include <cassert>
#include <functional>
#include <stdint.h>
#include "Memory.h"

class EmulatorPimpl {
public:
    EmulatorPimpl(RegisterFile *registers);

    size_t emulate();

    void set_memory(Memory *mem)
    {
        this->mem = mem;
    }

    void set_io(Memory *io)
    {
        this->io = io;
    }

    bool has_trapped() const
    {
        return executed_int3;
    }

    void reset();
private:
    uint8_t fetch_byte();
    //
    // mov
    //
    void mov88();
    void mov89();
    void mov8a();
    void mov8b();
    void movc6();
    void movc7();
    void movb0_b7();
    void movb8_bf();
    void mova0();
    void mova1();
    void mova2();
    void mova3();
    void mov8e();
    void mov8c();
    //
    // push
    //
    void pushff();
    void push50_57();
    void pushsr();
    //
    // pop
    //
    void popf8();
    void pop58_5f();
    void popsr();
    //
    // xchg
    //
    void xchg86();
    void xchg87();
    void xchg90_97();
    //
    // in
    //
    void ine4();
    void ine5();
    void inec();
    void ined();
    //
    // out
    //
    void oute6();
    void oute7();
    void outee();
    void outef();
    //
    // xlat
    //
    void xlatd7();
    //
    // lea
    //
    void lea8d();
    //
    // lds
    //
    void ldsc5();
    //
    // les
    //
    void lesc4();
    //
    // lahf
    //
    void lahf9f();
    //
    // sahf
    //
    void sahf9e();
    //
    // pushf
    //
    void pushf9c();
    //
    // popf
    //
    void popf9d();
    //
    // add / adc
    //
    void add_adc_sub_sbb_cmp_80();
    void add_adc_sub_sbb_cmp_81();
    void add_adc_sub_sbb_cmp_82();
    void add_adc_sub_sbb_cmp_83();
    //
    // add
    //
    template <typename T>
    std::pair<uint16_t, T> do_add(uint16_t v1, uint16_t v2,
                                  uint16_t carry_in=0);
    void add00();
    void add01();
    void add02();
    void add03();
    void add04();
    void add05();
    //
    // adc
    //
    void adc10();
    void adc11();
    void adc12();
    void adc13();
    void adc14();
    void adc15();
    //
    // sub
    //
    template <typename T>
    std::pair<uint16_t, T> do_sub(uint16_t v1, uint16_t v2,
                                  uint16_t carry_in=0);
    void sub28();
    void sub29();
    void sub2a();
    void sub2b();
    void sub2c();
    void sub2d();
    //
    //sbb
    //
    void sbb18();
    void sbb19();
    void sbb1a();
    void sbb1b();
    void sbb1c();
    void sbb1d();
    //
    // inc
    //
    void inc_dec_fe();
    void incff();
    void inc40_47();
    //
    // dec
    //
    void decff();
    void dec48_4f();
    //
    // ascii
    //
    void aaa37();
    void daa27();
    void aas3f();
    void das2f();
    void aamd4();
    //
    // neg
    //
    void negf6();
    void negf7();
    //
    // cmp
    //
    void cmp38();
    void cmp39();
    void cmp3a();
    void cmp3b();
    void cmp3c();
    void cmp3d();
    //
    // mul
    //
    void mulf6();
    void mulf7();
    //
    // imul
    //
    void imulf6();
    void imulf7();
    //
    // int
    //
    void intcc();
    //
    // cbw / cwd
    //
    void cbw98();
    void cwd99();
    //
    // jump
    //
    void jmpe9();
    void jmpeb();
    void jmpff_intra();
    void jmpff_inter();
    void jmpea();
    void je74();
    void jl7c();
    void jle7e();
    void jp7a();
    void jb72();
    void jbe76();
    void jo70();
    void js78();
    void jne75();
    void jnl7d();
    void jnle7f();
    void jnb73();
    void jnbe77();
    void jnp7b();
    void jno71();
    //
    // call
    //
    void calle8();
    void callff_intra();
    void callff_inter();
    void call9a();
    //
    // ret
    //
    void retc3();
    void retc2();
    void retcb();
    void retca();
    //
    // iret
    //
    void iretcf();
    template <typename T>
    std::pair<uint16_t, T> do_mul(int32_t v1, int32_t v2);
    // Helpers
    void push_inc_jmp_call_ff();
    void neg_mul_f6();
    void neg_mul_f7();
    void push_word(uint16_t v);
    uint16_t pop_word();
    template <typename T>
    std::pair<uint16_t, T> do_alu(int32_t v1, int32_t v2,
                                  int32_t carry_in,
                                  std::function<uint32_t(uint32_t, uint32_t, uint32_t)> alu_op);
    template <typename T>
        void write_data(T val, bool stack=false);
    template <typename T>
        T read_data(bool stack=false);
    uint16_t fetch_16bit();

    Memory *mem;
    Memory *io;
    RegisterFile *registers;
    size_t instr_length = 0;
    std::unique_ptr<ModRMDecoder> modrm_decoder;
    uint8_t opcode;
    bool executed_int3;
};

EmulatorPimpl::EmulatorPimpl(RegisterFile *registers)
    : registers(registers),
    executed_int3(false)
{
    modrm_decoder = std::make_unique<ModRMDecoder>(
        [&]{ return this->fetch_byte(); },
        this->registers
    );

    reset();
}

void EmulatorPimpl::reset()
{
    registers->reset();
    opcode = 0;
    instr_length = 0;
    executed_int3 = false;
}

size_t EmulatorPimpl::emulate()
{
    instr_length = 0;
    executed_int3 = false;
    auto orig_ip = registers->get(IP);

    opcode = fetch_byte();
    switch (opcode) {
    // mov
    case 0x88: mov88(); break;
    case 0x89: mov89(); break;
    case 0x8a: mov8a(); break;
    case 0x8b: mov8b(); break;
    case 0xc6: movc6(); break;
    case 0xc7: movc7(); break;
    case 0xb0 ... 0xb7: movb0_b7(); break;
    case 0xb8 ... 0xbf: movb8_bf(); break;
    case 0xa0: mova0(); break;
    case 0xa1: mova1(); break;
    case 0xa2: mova2(); break;
    case 0xa3: mova3(); break;
    case 0x8e: mov8e(); break;
    case 0x8c: mov8c(); break;
    // push
    case 0xff: push_inc_jmp_call_ff(); break;
    case 0x50 ... 0x57: push50_57(); break;
    case 0x06: // fallthrough
    case 0x0e: // fallthrough
    case 0x16: // fallthrough
    case 0x1e: pushsr(); break;
    // pop
    case 0xf8: popf8(); break;
    case 0x58 ... 0x5f: pop58_5f(); break;
    case 0x07: // fallthrough
    case 0x0f: // fallthrough
    case 0x17: // fallthrough
    case 0x1f: popsr(); break;
    // xchg
    case 0x86: xchg86(); break;
    case 0x87: xchg87(); break;
    case 0x90 ... 0x97: xchg90_97(); break;
    // in
    case 0xe4: ine4(); break;
    case 0xe5: ine5(); break;
    case 0xec: inec(); break;
    case 0xed: ined(); break;
    // out
    case 0xe6: oute6(); break;
    case 0xe7: oute7(); break;
    case 0xee: outee(); break;
    case 0xef: outef(); break;
    // xlat
    case 0xd7: xlatd7(); break;
    // lea
    case 0x8d: lea8d(); break;
    // lds
    case 0xc5: ldsc5(); break;
    // les
    case 0xc4: lesc4(); break;
    // lahf
    case 0x9f: lahf9f(); break;
    // sahf
    case 0x9e: sahf9e(); break;
    // pushf
    case 0x9c: pushf9c(); break;
    // popf
    case 0x9d: popf9d(); break;
    // add / adc / sub / sbb / cmp
    case 0x80: add_adc_sub_sbb_cmp_80(); break;
    case 0x81: add_adc_sub_sbb_cmp_81(); break;
    case 0x82: add_adc_sub_sbb_cmp_82(); break;
    case 0x83: add_adc_sub_sbb_cmp_83(); break;
    // add
    case 0x00: add00(); break;
    case 0x01: add01(); break;
    case 0x02: add02(); break;
    case 0x03: add03(); break;
    case 0x04: add04(); break;
    case 0x05: add05(); break;
    // adc
    case 0x10: adc10(); break;
    case 0x11: adc11(); break;
    case 0x12: adc12(); break;
    case 0x13: adc13(); break;
    case 0x14: adc14(); break;
    case 0x15: adc15(); break;
    // sub
    case 0x28: sub28(); break;
    case 0x29: sub29(); break;
    case 0x2a: sub2a(); break;
    case 0x2b: sub2b(); break;
    case 0x2c: sub2c(); break;
    case 0x2d: sub2d(); break;
    // sbb
    case 0x18: sbb18(); break;
    case 0x19: sbb19(); break;
    case 0x1a: sbb1a(); break;
    case 0x1b: sbb1b(); break;
    case 0x1c: sbb1c(); break;
    case 0x1d: sbb1d(); break;
    // inc
    case 0xfe: inc_dec_fe(); break;
    case 0x40 ... 0x47: inc40_47(); break;
    // dec
    case 0x48 ... 0x4f: dec48_4f(); break;
    // ascii
    case 0x37: aaa37(); break;
    case 0x27: daa27(); break;
    case 0x2f: das2f(); break;
    case 0x3f: aas3f(); break;
    case 0xd4: aamd4(); break;
    // neg
    case 0xf6: neg_mul_f6(); break;
    case 0xf7: neg_mul_f7(); break;
    // cmp
    case 0x38: cmp38(); break;
    case 0x39: cmp39(); break;
    case 0x3a: cmp3a(); break;
    case 0x3b: cmp3b(); break;
    case 0x3c: cmp3c(); break;
    case 0x3d: cmp3d(); break;
    // int
    case 0xcc:
        intcc();
        executed_int3 = true;
        break;
    // cbw
    case 0x98: cbw98(); break;
    // cwd
    case 0x99: cwd99(); break;
    // jmp
    case 0xe9: jmpe9(); break;
    case 0xea: jmpea(); break;
    case 0xeb: jmpeb(); break;
    case 0x70: jo70(); break;
    case 0x72: jb72(); break;
    case 0x74: je74(); break;
    case 0x76: jbe76(); break;
    case 0x78: js78(); break;
    case 0x7a: jp7a(); break;
    case 0x7c: jl7c(); break;
    case 0x7e: jle7e(); break;
    case 0x75: jne75(); break;
    case 0x7d: jnl7d(); break;
    case 0x7f: jnle7f(); break;
    case 0x73: jnb73(); break;
    case 0x77: jnbe77(); break;
    case 0x7b: jnp7b(); break;
    case 0x71: jno71(); break;
    // call
    case 0xe8: calle8(); break;
    case 0x9a: call9a(); break;
    // ret
    case 0xc3: retc3(); break;
    case 0xc2: retc2(); break;
    case 0xcb: retcb(); break;
    case 0xca: retca(); break;
    // iret
    case 0xcf: iretcf(); break;
    }

    if (registers->get(IP) == orig_ip)
        registers->set(IP, registers->get(IP) + instr_length);

    return instr_length;
}

// mov m/r, r (8-bit)
void EmulatorPimpl::mov88()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    auto source = modrm_decoder->reg();
    auto val = registers->get(source);

    write_data<uint8_t>(val);
}

// mov m/r, r (16-bit)
void EmulatorPimpl::mov89()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    auto source = modrm_decoder->reg();
    auto val = registers->get(source);

    write_data<uint16_t>(val);
}

// mov r, m/r (8-bit)
void EmulatorPimpl::mov8a()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t val = read_data<uint8_t>();

    auto dest = modrm_decoder->reg();
    registers->set(dest, val);
}

// mov r, m/r (16-bit)
void EmulatorPimpl::mov8b()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t val = read_data<uint16_t>();

    auto dest = modrm_decoder->reg();
    registers->set(dest, val);
}

// mov r/m, immediate (reg == 0), 8-bit
void EmulatorPimpl::movc6()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() == 0) {
        uint8_t immed = fetch_byte();
        write_data<uint8_t>(immed);
    }
}

// mov r/m, immediate (reg == 0), 16-bit
void EmulatorPimpl::movc7()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() == 0)
        write_data<uint16_t>(fetch_16bit());
}

// mov r, immediate, 8-bit
void EmulatorPimpl::movb0_b7()
{
    uint8_t immed = fetch_byte();
    auto reg = static_cast<GPR>(static_cast<int>(AL) + (opcode & 0x7));
    registers->set(reg, immed);
}

// mov r, immediate, 16-bit
void EmulatorPimpl::movb8_bf()
{
    uint16_t immed = fetch_16bit();
    auto reg = static_cast<GPR>(static_cast<int>(AX) + (opcode & 0x7));
    registers->set(reg, immed);
}

// mov al, m, 8-bit
void EmulatorPimpl::mova0()
{
    auto displacement = fetch_16bit();
    auto addr = get_phys_addr(registers->get(DS), displacement);
    auto val = mem->read<uint8_t>(addr);
    registers->set(AL, val);
}

// mov ax, m, 16-bit
void EmulatorPimpl::mova1()
{
    auto displacement = fetch_16bit();
    auto addr = get_phys_addr(registers->get(DS), displacement);
    auto val = mem->read<uint16_t>(addr);
    registers->set(AX, val);
}

// mov m, al 8-bit
void EmulatorPimpl::mova2()
{
    auto displacement = fetch_16bit();
    auto val = registers->get(AL);
    auto addr = get_phys_addr(registers->get(DS), displacement);
    mem->write<uint8_t>(addr, val);
}

// mov m, al 16-bit
void EmulatorPimpl::mova3()
{
    auto displacement = fetch_16bit();
    auto val = registers->get(AX);
    auto addr = get_phys_addr(registers->get(DS), displacement);
    mem->write<uint16_t>(addr, val);
}

// mov sr, r/m
void EmulatorPimpl::mov8e()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() & (1 << 2))
        return;

    uint16_t val = read_data<uint16_t>();
    auto segnum = modrm_decoder->raw_reg();
    auto reg = static_cast<GPR>(static_cast<int>(ES) + segnum);

    registers->set(reg, val);
}

// mov r/m, sr
void EmulatorPimpl::mov8c()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() & (1 << 2))
        return;

    auto segnum = modrm_decoder->raw_reg();
    auto reg = static_cast<GPR>(static_cast<int>(ES) + segnum);
    uint16_t val = registers->get(reg);

    write_data<uint16_t>(val);
}

void EmulatorPimpl::push_inc_jmp_call_ff()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() == 6)
        pushff();
    else if (modrm_decoder->raw_reg() == 0)
        incff();
    else if (modrm_decoder->raw_reg() == 1)
        decff();
    else if (modrm_decoder->raw_reg() == 2)
        callff_intra();
    else if (modrm_decoder->raw_reg() == 3)
        callff_inter();
    else if (modrm_decoder->raw_reg() == 4)
        jmpff_intra();
    else if (modrm_decoder->raw_reg() == 5)
        jmpff_inter();
}

// push r/m
void EmulatorPimpl::pushff()
{
    assert(modrm_decoder->raw_reg() == 6);

    auto val = read_data<uint16_t>();
    registers->set(SP, registers->get(SP) - 2);
    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    mem->write<uint16_t>(addr, val);
}

// push r
void EmulatorPimpl::push50_57()
{
    auto reg = static_cast<GPR>(static_cast<int>(AX) + (opcode & 0x7));
    auto val = registers->get(reg);

    registers->set(SP, registers->get(SP) - 2);
    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    mem->write<uint16_t>(addr, val);
}

// push sr
void EmulatorPimpl::pushsr()
{
    auto reg = static_cast<GPR>(static_cast<int>(ES) + ((opcode >> 3) & 0x3));
    auto val = registers->get(reg);

    registers->set(SP, registers->get(SP) - 2);
    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    mem->write<uint16_t>(addr, val);
}

// pop r/m
void EmulatorPimpl::popf8()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() != 0)
        return;

    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    auto val = mem->read<uint16_t>(addr);
    registers->set(SP, registers->get(SP) + 2);

    write_data<uint16_t>(val);
}

// pop r
void EmulatorPimpl::pop58_5f()
{
    auto reg = static_cast<GPR>(static_cast<int>(AX) + (opcode & 0x7));
    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    auto val = mem->read<uint16_t>(addr);

    registers->set(SP, registers->get(SP) + 2);
    registers->set(reg, val);
}

// pop sr
void EmulatorPimpl::popsr()
{
    auto reg = static_cast<GPR>(static_cast<int>(ES) + ((opcode >> 3) & 0x3));
    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    auto val = mem->read<uint16_t>(addr);

    registers->set(reg, val);
    registers->set(SP, registers->get(SP) + 2);
}

// xchg r, r/m, 8-bit
void EmulatorPimpl::xchg86()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    auto v1 = read_data<uint8_t>();
    auto v2 = registers->get(modrm_decoder->reg());

    write_data<uint8_t>(v2);
    registers->set(modrm_decoder->reg(), v1);
}

// xchg r, r/m, 16-bit
void EmulatorPimpl::xchg87()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    auto v1 = read_data<uint16_t>();
    auto v2 = registers->get(modrm_decoder->reg());

    write_data<uint16_t>(v2);
    registers->set(modrm_decoder->reg(), v1);
}

// xchg accumulator, r
void EmulatorPimpl::xchg90_97()
{
    auto reg = static_cast<GPR>(static_cast<int>(AX) + (opcode & 0x7));

    auto v1 = registers->get(AX);
    auto v2 = registers->get(reg);

    registers->set(AX, v2);
    registers->set(reg, v1);
}

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

// xlat
void EmulatorPimpl::xlatd7()
{
    auto v = registers->get(AL);
    auto table_addr = registers->get(BX);
    auto xlated_val = mem->read<uint8_t>(get_phys_addr(registers->get(DS),
                                                       table_addr + v));
    registers->set(AL, xlated_val);
}

// lea r, r/m
void EmulatorPimpl::lea8d()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    registers->set(modrm_decoder->reg(), modrm_decoder->effective_address());
}

// lds r, m
void EmulatorPimpl::ldsc5()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    if (modrm_decoder->rm_type() == OP_REG)
        return;

    auto p32 = get_phys_addr(registers->get(DS), modrm_decoder->effective_address());
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

    auto p32 = get_phys_addr(registers->get(DS), modrm_decoder->effective_address());
    auto displacement = mem->read<uint16_t>(p32);
    auto seg = mem->read<uint16_t>(p32 + 2);

    registers->set(modrm_decoder->reg(), displacement);
    registers->set(ES, seg);
}

// lahf
void EmulatorPimpl::lahf9f()
{
    auto flags = registers->get_flags();
    registers->set(AH, flags & 0xff);
}

// sahf
void EmulatorPimpl::sahf9e()
{
    auto new_flags = registers->get(AH);
    auto old_flags = registers->get_flags();
    registers->set_flags((old_flags & 0xff00) | new_flags);
}

// pushf
void EmulatorPimpl::pushf9c()
{
    auto val = registers->get_flags();
    registers->set(SP, registers->get(SP) - 2);
    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    mem->write<uint16_t>(addr, val);
}

// popf
void EmulatorPimpl::popf9d()
{
    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    auto val = mem->read<uint16_t>(addr);
    registers->set_flags(val);

    registers->set(SP, registers->get(SP) + 2);
}

template <typename T>
std::pair<uint16_t, T>
EmulatorPimpl::do_alu(int32_t v1, int32_t v2, int32_t carry,
                      std::function<uint32_t(uint32_t, uint32_t, uint32_t)> alu_op)
{
    uint16_t flags = registers->get_flags();

    flags &= ~(AF | CF | OF | PF | SF | ZF);

    uint32_t result32 = alu_op(static_cast<uint32_t>(v1),
                               static_cast<uint32_t>(v2),
                               static_cast<uint32_t>(carry));
    bool af = !!(alu_op(v1 & 0xf, v2 & 0xf, 0) & (1 << 4));

    auto sign_bit = (8 * sizeof(T)) - 1;
    auto carry_bit = (8 * sizeof(T));

    if (af)
        flags |= AF;
    if (result32 & (1 << carry_bit))
        flags |= CF;
    if (result32 & (1 << sign_bit))
        flags |= SF;
    if ((result32 & static_cast<T>(-1)) == 0)
        flags |= ZF;
    if (!__builtin_parity(result32 & static_cast<uint8_t>(-1)))
        flags |= PF;
    bool carry_in = !!(alu_op(static_cast<uint32_t>(v1) & ~(1 << sign_bit),
                              static_cast<uint32_t>(v2) & ~(1 << sign_bit),
                              static_cast<uint32_t>(carry)) & (1 << sign_bit));
    if (carry_in ^ !!(flags & CF))
        flags |= OF;

    return std::make_pair(flags, static_cast<T>(result32));
}

template <typename T>
std::pair<uint16_t, T> EmulatorPimpl::do_add(uint16_t v1, uint16_t v2,
                                             uint16_t carry_in)
{
    return do_alu<T>(v1, v2, carry_in,
        [](uint32_t a, uint32_t b, uint32_t c) -> uint32_t {
            return a + b + c;
        });
}

template <typename T>
std::pair<uint16_t, T> EmulatorPimpl::do_sub(uint16_t v1, uint16_t v2,
                                             uint16_t carry_in)
{
    return do_alu<T>(v1, v2, carry_in,
        [](uint32_t a, uint32_t b, uint32_t c) -> uint32_t {
            return a - b - c;
        });
}

template <typename T>
std::pair<uint16_t, T> EmulatorPimpl::do_mul(int32_t v1, int32_t v2)
{
    return do_alu<T>(v1, v2, 0,
        [](uint32_t a, uint32_t b, uint32_t __attribute__((unused)) c) -> uint32_t {
            return a * b;
        });
}

// add r, r/m, 8-bit
void EmulatorPimpl::add00()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = registers->get(modrm_decoder->reg());

    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_add<uint8_t>(v1, v2);

    registers->set_flags(flags);
    write_data<uint8_t>(result & 0xff);
}

// add r, r/m, 16-bit
void EmulatorPimpl::add01()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = registers->get(modrm_decoder->reg());

    uint16_t result, flags;
    std::tie(flags, result) = do_add<uint16_t>(v1, v2);

    registers->set_flags(flags);
    write_data<uint16_t>(result & 0xffff);
}

// add r/m, r, 8-bit
void EmulatorPimpl::add02()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = registers->get(modrm_decoder->reg());

    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_add<uint8_t>(v1, v2);

    registers->set_flags(flags);
    registers->set(modrm_decoder->reg(), result & 0xff);
}

// add r/m, r, 16-bit
void EmulatorPimpl::add03()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = registers->get(modrm_decoder->reg());

    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_add<uint16_t>(v1, v2);

    registers->set_flags(flags);
    registers->set(modrm_decoder->reg(), result & 0xffff);
}

void EmulatorPimpl::add_adc_sub_sbb_cmp_80()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() != 0 &&
        modrm_decoder->raw_reg() != 2 &&
        modrm_decoder->raw_reg() != 5 &&
        modrm_decoder->raw_reg() != 3 &&
        modrm_decoder->raw_reg() != 7)
        return;

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = fetch_byte();
    bool carry_in = modrm_decoder->raw_reg() == 2 || modrm_decoder->raw_reg() == 3 ?
        !!(registers->get_flags() & CF) : 0;

    uint8_t result;
    uint16_t flags;
    if (modrm_decoder->raw_reg() == 0 ||
        modrm_decoder->raw_reg() == 2)
        std::tie(flags, result) = do_add<uint8_t>(v1, v2, carry_in);
    else
        std::tie(flags, result) = do_sub<uint8_t>(v1, v2, carry_in);

    registers->set_flags(flags);
    // cmp doesn't write the result
    if (modrm_decoder->raw_reg() != 7)
        write_data<uint8_t>(result & 0xff);
}

void EmulatorPimpl::add_adc_sub_sbb_cmp_82()
{
    // The 's' bit has no effect for 8-bit add immediate.
    add_adc_sub_sbb_cmp_80();
}

// add r/m, immediate, 16-bit
void EmulatorPimpl::add_adc_sub_sbb_cmp_81()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() != 0 &&
        modrm_decoder->raw_reg() != 2 &&
        modrm_decoder->raw_reg() != 5 &&
        modrm_decoder->raw_reg() != 3 &&
        modrm_decoder->raw_reg() != 7)
        return;

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = fetch_16bit();
    bool carry_in = modrm_decoder->raw_reg() == 2 || modrm_decoder->raw_reg() == 3 ?
        !!(registers->get_flags() & CF) : 0;

    uint16_t result;
    uint16_t flags;
    if (modrm_decoder->raw_reg() == 0 ||
        modrm_decoder->raw_reg() == 2)
        std::tie(flags, result) = do_add<uint16_t>(v1, v2, carry_in);
    else
        std::tie(flags, result) = do_sub<uint16_t>(v1, v2, carry_in);

    registers->set_flags(flags);
    // cmp doesn't write the result
    if (modrm_decoder->raw_reg() != 7)
        write_data<uint16_t>(result & 0xffff);
}

// add r/m, immediate, 8-bit, sign-extended
void EmulatorPimpl::add_adc_sub_sbb_cmp_83()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() != 0 &&
        modrm_decoder->raw_reg() != 2 &&
        modrm_decoder->raw_reg() != 5 &&
        modrm_decoder->raw_reg() != 3 &&
        modrm_decoder->raw_reg() != 7)
        return;

    uint16_t v1 = read_data<uint16_t>();
    int16_t immed = fetch_byte();
    immed <<= 8;
    immed >>= 8;
    bool carry_in = modrm_decoder->raw_reg() == 2 || modrm_decoder->raw_reg() == 3 ?
        !!(registers->get_flags() & CF) : 0;

    uint16_t result;
    uint16_t flags;
    if (modrm_decoder->raw_reg() == 0 ||
        modrm_decoder->raw_reg() == 2)
        std::tie(flags, result) = do_add<uint16_t>(v1, immed, carry_in);
    else
        std::tie(flags, result) = do_sub<uint16_t>(v1, immed, carry_in);

    registers->set_flags(flags);
    // cmp doesn't write the result
    if (modrm_decoder->raw_reg() != 7)
        write_data<uint16_t>(result & 0xffff);
}

void EmulatorPimpl::add04()
{
    auto v1 = registers->get(AL);
    auto v2 = fetch_byte();
    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_add<uint8_t>(v1, v2);

    registers->set_flags(flags);
    registers->set(AL, result);
}

void EmulatorPimpl::add05()
{
    auto v1 = registers->get(AX);
    auto v2 = fetch_16bit();
    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_add<uint16_t>(v1, v2);

    registers->set_flags(flags);
    registers->set(AX, result);
}

// adc r, r/m, 8-bit
void EmulatorPimpl::adc10()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = registers->get(modrm_decoder->reg());
    bool carry_in = !!(registers->get_flags() & CF);

    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_add<uint8_t>(v1, v2, carry_in);

    registers->set_flags(flags);
    write_data<uint8_t>(result & 0xff);
}

// adc r, r/m, 16-bit
void EmulatorPimpl::adc11()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = registers->get(modrm_decoder->reg());
    bool carry_in = !!(registers->get_flags() & CF);

    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_add<uint16_t>(v1, v2, carry_in);

    registers->set_flags(flags);
    write_data<uint16_t>(result & 0xffff);
}

// adc r/m, r, 8-bit
void EmulatorPimpl::adc12()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = registers->get(modrm_decoder->reg());
    bool carry_in = !!(registers->get_flags() & CF);

    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_add<uint8_t>(v1, v2, carry_in);

    registers->set_flags(flags);
    registers->set(modrm_decoder->reg(), result & 0xff);
}

// adc r/m, r, 16-bit
void EmulatorPimpl::adc13()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = registers->get(modrm_decoder->reg());
    bool carry_in = !!(registers->get_flags() & CF);

    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_add<uint16_t>(v1, v2, carry_in);

    registers->set_flags(flags);
    registers->set(modrm_decoder->reg(), result & 0xffff);
}

void EmulatorPimpl::adc14()
{
    auto v1 = registers->get(AL);
    auto v2 = fetch_byte();
    bool carry_in = !!(registers->get_flags() & CF);
    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_add<uint8_t>(v1, v2, carry_in);

    registers->set_flags(flags);
    registers->set(AL, result);
}

void EmulatorPimpl::adc15()
{
    auto v1 = registers->get(AX);
    auto v2 = fetch_16bit();
    bool carry_in = !!(registers->get_flags() & CF);
    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_add<uint8_t>(v1, v2, carry_in);

    registers->set_flags(flags);
    registers->set(AX, result);
}

// sub r, r/m, 8-bit
void EmulatorPimpl::sub28()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = registers->get(modrm_decoder->reg());

    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint8_t>(v1, v2);

    registers->set_flags(flags);
    write_data<uint8_t>(result & 0xff);
}

// sub r, r/m, 16-bit
void EmulatorPimpl::sub29()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = registers->get(modrm_decoder->reg());

    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint16_t>(v1, v2);

    registers->set_flags(flags);
    write_data<uint16_t>(result & 0xffff);
}

// sub r/m, r, 8-bit
void EmulatorPimpl::sub2a()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = registers->get(modrm_decoder->reg());

    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint8_t>(v2, v1);

    registers->set_flags(flags);
    registers->set(modrm_decoder->reg(), result & 0xff);
}

// sub r/m, r, 16-bit
void EmulatorPimpl::sub2b()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = registers->get(modrm_decoder->reg());

    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint16_t>(v2, v1);

    registers->set_flags(flags);
    registers->set(modrm_decoder->reg(), result & 0xffff);
}

void EmulatorPimpl::sub2c()
{
    auto v1 = registers->get(AL);
    auto v2 = fetch_byte();
    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint8_t>(v1, v2);

    registers->set_flags(flags);
    registers->set(AL, result);
}

void EmulatorPimpl::sub2d()
{
    auto v1 = registers->get(AX);
    auto v2 = fetch_16bit();
    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint16_t>(v1, v2);

    registers->set_flags(flags);
    registers->set(AX, result);
}

void EmulatorPimpl::sbb18()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = registers->get(modrm_decoder->reg());
    bool carry_in = !!(registers->get_flags() & CF);

    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint8_t>(v1, v2, carry_in);

    registers->set_flags(flags);
    write_data<uint8_t>(result & 0xff);
}

// sbb r, r/m, 16-bit
void EmulatorPimpl::sbb19()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = registers->get(modrm_decoder->reg());
    bool carry_in = !!(registers->get_flags() & CF);

    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint16_t>(v1, v2, carry_in);

    registers->set_flags(flags);
    write_data<uint16_t>(result & 0xffff);
}

// sbb r/m, r, 8-bit
void EmulatorPimpl::sbb1a()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = registers->get(modrm_decoder->reg());
    bool carry_in = !!(registers->get_flags() & CF);

    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint8_t>(v2, v1, carry_in);

    registers->set_flags(flags);
    registers->set(modrm_decoder->reg(), result & 0xff);
}

// sbb r/m, r, 16-bit
void EmulatorPimpl::sbb1b()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = registers->get(modrm_decoder->reg());
    bool carry_in = !!(registers->get_flags() & CF);

    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint16_t>(v2, v1, carry_in);

    registers->set_flags(flags);
    registers->set(modrm_decoder->reg(), result & 0xffff);
}

void EmulatorPimpl::sbb1c()
{
    auto v1 = registers->get(AL);
    auto v2 = fetch_byte();
    bool carry_in = !!(registers->get_flags() & CF);
    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint8_t>(v1, v2, carry_in);

    registers->set_flags(flags);
    registers->set(AL, result);
}

void EmulatorPimpl::sbb1d()
{
    auto v1 = registers->get(AX);
    auto v2 = fetch_16bit();
    bool carry_in = !!(registers->get_flags() & CF);
    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint16_t>(v1, v2, carry_in);

    registers->set_flags(flags);
    registers->set(AX, result);
}

// inc r/m, 8-bit
void EmulatorPimpl::inc_dec_fe()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() != 0 &&
        modrm_decoder->raw_reg() != 1)
        return;

    uint8_t v = read_data<uint8_t>();
    uint8_t result;
    uint16_t flags, old_flags = registers->get_flags();

    if (modrm_decoder->raw_reg() == 0)
        std::tie(flags, result) = do_add<uint8_t>(v, 1);
    else
        std::tie(flags, result) = do_sub<uint8_t>(v, 1);

    old_flags &= ~(OF | SF | ZF | AF | PF);
    flags &= (OF | SF | ZF | AF | PF);
    registers->set_flags(flags | old_flags);
    write_data<uint8_t>(result);
}

// inc r/m, 16-bit
void EmulatorPimpl::incff()
{
    assert(modrm_decoder->raw_reg() == 0);

    uint16_t v = read_data<uint16_t>();
    uint16_t result;
    uint16_t flags, old_flags = registers->get_flags();

    std::tie(flags, result) = do_add<uint16_t>(v, 1);

    old_flags &= ~(OF | SF | ZF | AF | PF);
    flags &= (OF | SF | ZF | AF | PF);
    registers->set_flags(flags | old_flags);
    write_data<uint16_t>(result);
}

// inc r, 16-bit
void EmulatorPimpl::inc40_47()
{
    auto reg = static_cast<GPR>(static_cast<int>(AX) + (opcode & 0x7));
    auto v = registers->get(reg);
    uint16_t result;
    uint16_t flags, old_flags = registers->get_flags();

    std::tie(flags, result) = do_add<uint16_t>(v, 1);

    old_flags &= ~(OF | SF | ZF | AF | PF);
    flags &= (OF | SF | ZF | AF | PF);
    registers->set_flags(flags | old_flags);
    registers->set(reg, result);
}

// dec r/m, 16-bit
void EmulatorPimpl::decff()
{
    assert(modrm_decoder->raw_reg() == 1);

    uint16_t v = read_data<uint16_t>();
    uint16_t result;
    uint16_t flags, old_flags = registers->get_flags();

    std::tie(flags, result) = do_sub<uint16_t>(v, 1);

    old_flags &= ~(OF | SF | ZF | AF | PF);
    flags &= (OF | SF | ZF | AF | PF);
    registers->set_flags(flags | old_flags);
    write_data<uint16_t>(result);
}

uint8_t EmulatorPimpl::fetch_byte()
{
    return mem->read<uint8_t>(get_phys_addr(registers->get(CS),
                                            registers->get(IP) + instr_length++));
}

// dec r, 16-bit
void EmulatorPimpl::dec48_4f()
{
    auto reg = static_cast<GPR>(static_cast<int>(AX) + (opcode & 0x7));
    auto v = registers->get(reg);
    uint16_t result;
    uint16_t flags, old_flags = registers->get_flags();

    std::tie(flags, result) = do_sub<uint16_t>(v, 1);

    old_flags &= ~(OF | SF | ZF | AF | PF);
    flags &= (OF | SF | ZF | AF | PF);
    registers->set_flags(flags | old_flags);
    registers->set(reg, result);
}

// aaa
void EmulatorPimpl::aaa37()
{
    uint16_t flags = registers->get_flags();

    auto al = registers->get(AL);
    if ((al & 0x0f) > 9 || (flags & AF)) {
        al = (al + 6) & 0x0f;
        registers->set(AL, al);

        auto ah = registers->get(AH);
        ah++;
        registers->set(AH, ah);

        flags |= AF | CF;
    } else {
        flags &= ~(AF | CF);
    }

    registers->set_flags(flags);
}

// daa
void EmulatorPimpl::daa27()
{
    uint16_t flags = registers->get_flags();

    auto al = registers->get(AL);
    if ((al & 0x0f) > 9 || (flags & AF)) {
        al = (al + 6);
        flags |= AF;
    }

    if (al > 0x9f || (flags & CF)) {
        al += 0x60;
        flags |= CF;
    }

    registers->set(AL, al);
    registers->set_flags(flags);
}

// aas
void EmulatorPimpl::aas3f()
{
    uint16_t flags = registers->get_flags();

    auto al = registers->get(AL);
    auto ah = registers->get(AH);
    if ((al & 0x0f) > 9 || (flags & AF)) {
        al -= 6;
        al &= 0xf;
        --ah;

        flags |= AF | CF;
    } else {
        flags &= ~(AF | CF);
    }

    registers->set(AL, al);
    registers->set(AH, ah);
    registers->set_flags(flags);
}

// aam
void EmulatorPimpl::aamd4()
{
    auto divisor = fetch_byte();

    auto al = registers->get(AL);
    auto quotient = al / divisor;
    auto remainder = al % divisor;

    registers->set(AH, quotient);
    registers->set(AL, remainder);

    uint16_t flags = registers->get_flags();
    flags &= ~(PF | SF | ZF);

    if (!(registers->get(AL) & 0xff))
        flags |= ZF;
    if (!__builtin_parity(registers->get(AL) & 0xff))
        flags |= PF;
    if (registers->get(AL) & 0x80)
        flags |= SF;

    registers->set_flags(flags);
}

// das
void EmulatorPimpl::das2f()
{
    uint16_t flags = registers->get_flags();

    auto al = registers->get(AL);
    auto ah = registers->get(AH);
    if ((al & 0x0f) > 9 || (flags & AF)) {
        al -= 6;
        flags |= AF;
    } else {
        flags &= ~AF;
    }

    if (al > 0x9f || (flags & CF)) {
        al -= 0x60;
        flags |= CF;
    } else {
        flags &= ~CF;
    }

    if (al & 0x80)
        flags |= SF;
    if (!al)
        flags |= ZF;
    if (!__builtin_parity(al))
        flags |= PF;

    registers->set(AL, al);
    registers->set(AH, ah);
    registers->set_flags(flags);
}

void EmulatorPimpl::neg_mul_f6()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() == 0x3)
        negf6();
    else if (modrm_decoder->raw_reg() == 0x4)
        mulf6();
    else if (modrm_decoder->raw_reg() == 0x5)
        imulf6();
}

// neg byte r/m
void EmulatorPimpl::negf6()
{
    auto v = read_data<uint8_t>();
    uint16_t flags;
    uint8_t result;
    std::tie(flags, result) = do_sub<uint8_t>(0, v);

    write_data<uint8_t>(result);
    registers->set_flags(flags);
}

void EmulatorPimpl::neg_mul_f7()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() == 0x3)
        negf7();
    else if (modrm_decoder->raw_reg() == 0x4)
        mulf7();
    else if (modrm_decoder->raw_reg() == 0x5)
        imulf7();
}

// neg word r/m
void EmulatorPimpl::negf7()
{
    auto v = read_data<uint16_t>();
    uint16_t flags;
    uint16_t result;
    std::tie(flags, result) = do_sub<uint16_t>(0, v);

    write_data<uint16_t>(result);
    registers->set_flags(flags);
}

// cmp r, r/m, 8-bit
void EmulatorPimpl::cmp38()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = registers->get(modrm_decoder->reg());

    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint8_t>(v1, v2);

    registers->set_flags(flags);
}

// cmp r, r/m, 16-bit
void EmulatorPimpl::cmp39()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = registers->get(modrm_decoder->reg());

    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint16_t>(v1, v2);

    registers->set_flags(flags);
}

// cmp r/m, r, 8-bit
void EmulatorPimpl::cmp3a()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = registers->get(modrm_decoder->reg());

    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint8_t>(v2, v1);

    registers->set_flags(flags);
}

// cmp r/m, r, 16-bit
void EmulatorPimpl::cmp3b()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = registers->get(modrm_decoder->reg());

    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint16_t>(v2, v1);

    registers->set_flags(flags);
}

void EmulatorPimpl::cmp3c()
{
    auto v1 = registers->get(AL);
    auto v2 = fetch_byte();
    uint8_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint8_t>(v1, v2);

    registers->set_flags(flags);
}

void EmulatorPimpl::cmp3d()
{
    auto v1 = registers->get(AX);
    auto v2 = fetch_16bit();
    uint16_t result;
    uint16_t flags;
    std::tie(flags, result) = do_sub<uint16_t>(v1, v2);

    registers->set_flags(flags);
}

// mul r/m, 8-bit
void EmulatorPimpl::mulf6()
{
    auto old_flags = registers->get_flags();
    auto v1 = read_data<uint8_t>();
    auto v2 = registers->get(AL);

    uint16_t result, flags;
    std::tie(flags, result) = do_mul<uint16_t>(v1, v2);

    flags = old_flags & ~(CF | OF);
    if (result & 0xff00)
        flags |= CF | OF;

    registers->set(AX, result);
    registers->set_flags(flags);
}

// mul r/m, 16-bit
void EmulatorPimpl::mulf7()
{
    auto old_flags = registers->get_flags();
    auto v1 = read_data<uint16_t>();
    auto v2 = registers->get(AX);

    uint32_t result;
    uint16_t flags;
    std::tie(flags, result) = do_mul<uint32_t>(v1, v2);

    flags = old_flags & ~(CF | OF);
    if (result & 0xffff0000)
        flags |= CF | OF;

    registers->set(AX, result & 0xffff);
    registers->set(DX, (result >> 16) & 0xffff);
    registers->set_flags(flags);
}

template <typename Out, typename In>
static inline Out sign_extend(In v)
{
    static_assert(sizeof(Out) > sizeof(In), "May only sign extend to larger types");

    size_t bit_shift = (sizeof(Out) - sizeof(In)) * 8;
    Out o = static_cast<Out>(v);

    o <<= bit_shift;
    o >>= bit_shift;

    return o;
}

// imul r/m, 8-bit
void EmulatorPimpl::imulf6()
{
    auto old_flags = registers->get_flags();
    int16_t v1 = sign_extend<int16_t, uint8_t>(read_data<uint8_t>());
    int16_t v2 = sign_extend<int16_t, uint8_t>(registers->get(AL));

    uint16_t result, flags;
    std::tie(flags, result) = do_mul<int16_t>(v1, v2);
    flags = old_flags & ~(CF | OF);
    if ((result & 0xff80) != 0xff80 &&
        (result & 0xff80) != 0x0000)
        flags |= CF | OF;

    registers->set(AX, result);
    registers->set_flags(flags);
}

// imul r/m, 16-bit
void EmulatorPimpl::imulf7()
{
    auto old_flags = registers->get_flags();
    int32_t v1 = sign_extend<int32_t, uint16_t>(read_data<uint16_t>());
    int32_t v2 = sign_extend<int32_t, uint16_t>(registers->get(AX));

    uint32_t result;
    uint16_t flags;
    std::tie(flags, result) = do_mul<int32_t>(v1, v2);
    flags = old_flags & ~(CF | OF);
    if ((result & 0xffff8000) != 0xffff8000 &&
        (result & 0xffff8000) != 0x00000000)
        flags |= CF | OF;

    registers->set(AX, result & 0xffff);
    registers->set(DX, (result >> 16) & 0xffff);
    registers->set_flags(flags);
}

void EmulatorPimpl::intcc()
{
    auto flags = registers->get_flags();

    push_word(flags);
    push_word(registers->get(CS));
    push_word(registers->get(IP) + instr_length);

    flags &= ~(IF | TF);
    registers->set_flags(flags);

    // int 3
    auto new_cs = mem->read<uint16_t>(4 * 3 + 2);
    auto new_ip = mem->read<uint16_t>(4 * 3 + 0);

    registers->set(CS, new_cs);
    registers->set(IP, new_ip);
}

void EmulatorPimpl::cbw98()
{
    registers->set(AX, sign_extend<int16_t, uint8_t>(registers->get(AL)));
}

void EmulatorPimpl::cwd99()
{
    int32_t v = sign_extend<int32_t, uint16_t>(registers->get(AX));
    registers->set(DX, (v >> 16) & 0xffff);
}

void EmulatorPimpl::jmpe9()
{
    int16_t displacement = static_cast<int16_t>(fetch_16bit());

    registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jmpea()
{
    uint16_t ip = fetch_16bit();
    uint16_t cs = fetch_16bit();

    registers->set(IP, ip);
    registers->set(CS, cs);
}

void EmulatorPimpl::jmpeb()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::je74()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (registers->get_flags() & ZF)
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jl7c()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (!!(registers->get_flags() & OF) ^ !!(registers->get_flags() & SF))
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jle7e()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if ((!!(registers->get_flags() & OF) ^ !!(registers->get_flags() & SF)) ||
        (registers->get_flags() & ZF))
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jb72()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (registers->get_flags() & CF)
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jbe76()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if ((registers->get_flags() & CF) || (registers->get_flags() & ZF))
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jp7a()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (registers->get_flags() & PF)
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jo70()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (registers->get_flags() & OF)
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::js78()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (registers->get_flags() & SF)
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jne75()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (!(registers->get_flags() & ZF))
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jnl7d()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if ((!!(registers->get_flags() & SF) ^ !!(registers->get_flags() & OF)) == 0)
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jnle7f()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (((registers->get_flag(SF) ^ registers->get_flag(OF)) | registers->get_flag(ZF)) == 0)
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jnb73()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (!registers->get_flag(CF))
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jnbe77()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if ((registers->get_flag(CF) || registers->get_flag(ZF)) == 0)
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jnp7b()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (!registers->get_flag(PF))
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jno71()
{
    int8_t displacement = static_cast<int8_t>(fetch_byte());

    if (!registers->get_flag(OF))
        registers->set(IP, registers->get(IP) + displacement + instr_length);
}

void EmulatorPimpl::jmpff_intra()
{
    auto ip = read_data<uint16_t>();

    registers->set(IP, ip);
}

void EmulatorPimpl::jmpff_inter()
{
    if (modrm_decoder->rm_type() == OP_REG)
        return;

    auto cs_ip = read_data<uint32_t>();

    registers->set(IP, cs_ip & 0xffff);
    registers->set(CS, (cs_ip >> 16) & 0xffff);
}

void EmulatorPimpl::calle8()
{
    auto displacement = fetch_16bit();

    push_word(registers->get(IP) + instr_length);

    registers->set(IP, displacement);
}

void EmulatorPimpl::callff_intra()
{
    auto displacement = read_data<uint16_t>();

    push_word(registers->get(IP) + instr_length);

    registers->set(IP, displacement);
}

void EmulatorPimpl::callff_inter()
{
    if (modrm_decoder->rm_type() == OP_REG)
        return;

    push_word(registers->get(CS));
    push_word(registers->get(IP) + instr_length);

    auto cs_ip = read_data<uint32_t>();

    registers->set(IP, cs_ip & 0xffff);
    registers->set(CS, (cs_ip >> 16) & 0xffff);
}

void EmulatorPimpl::call9a()
{
    auto new_ip = fetch_16bit();
    auto new_cs = fetch_16bit();

    push_word(registers->get(CS));
    push_word(registers->get(IP) + instr_length);

    registers->set(CS, new_cs);
    registers->set(IP, new_ip);
}

void EmulatorPimpl::retc3()
{
    auto ip = pop_word();

    registers->set(IP, ip);
}

void EmulatorPimpl::retc2()
{
    auto displacement = fetch_16bit();
    auto ip = pop_word();

    registers->set(IP, ip);
    registers->set(SP, registers->get(SP) + displacement);
}

void EmulatorPimpl::retcb()
{
    auto ip = pop_word();
    auto cs = pop_word();

    registers->set(IP, ip);
    registers->set(CS, cs);
}

void EmulatorPimpl::retca()
{
    auto displacement = fetch_16bit();
    auto ip = pop_word();
    auto cs = pop_word();

    registers->set(IP, ip);
    registers->set(CS, cs);
    registers->set(SP, registers->get(SP) + displacement);
}

void EmulatorPimpl::iretcf()
{
    auto ip = pop_word();
    auto cs = pop_word();
    auto flags = pop_word();

    registers->set(IP, ip);
    registers->set(CS, cs);
    registers->set_flags(flags);
}

void EmulatorPimpl::push_word(uint16_t v)
{
    registers->set(SP, registers->get(SP) - 2);
    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    mem->write<uint16_t>(addr, v);
}

uint16_t EmulatorPimpl::pop_word()
{
    auto addr = get_phys_addr(registers->get(SS), registers->get(SP));
    auto v = mem->read<uint16_t>(addr);
    registers->set(SP, registers->get(SP) + 2);

    return v;
}

uint16_t EmulatorPimpl::fetch_16bit()
{
    uint16_t immed = (static_cast<uint16_t>(fetch_byte()) |
                      (static_cast<uint16_t>(fetch_byte()) << 8));
    return immed;
}

template <typename T>
void EmulatorPimpl::write_data(T val, bool stack)
{
    if (modrm_decoder->rm_type() == OP_REG) {
        auto dest = modrm_decoder->rm_reg();
        registers->set(dest, val);
    } else {
        auto ea = modrm_decoder->effective_address();
        auto segment = modrm_decoder->uses_bp_as_base() || stack ? SS : DS;
        auto addr = get_phys_addr(registers->get(segment), ea);
        mem->write<T>(addr, val);
    }
}

template <typename T>
T EmulatorPimpl::read_data(bool stack)
{
    if (modrm_decoder->rm_type() == OP_MEM) {
        auto displacement = modrm_decoder->effective_address();
        auto segment = modrm_decoder->uses_bp_as_base() || stack ? SS : DS;
        auto addr = get_phys_addr(registers->get(segment), displacement);

        return mem->read<T>(addr);
    } else {
        auto source = modrm_decoder->rm_reg();
        return registers->get(source);
    }
}

Emulator::Emulator(RegisterFile *registers)
    : pimpl(std::make_unique<EmulatorPimpl>(registers))
{
}

Emulator::~Emulator()
{
}

size_t Emulator::emulate()
{
    return pimpl->emulate();
}

void Emulator::set_memory(Memory *mem)
{
    pimpl->set_memory(mem);
}

void Emulator::set_io(Memory *io)
{
    pimpl->set_io(io);
}

bool Emulator::has_trapped() const
{
    return pimpl->has_trapped();
}

void Emulator::reset()
{
    pimpl->reset();
}
