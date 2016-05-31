#include "Emulate.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <functional>
#include <stdint.h>
#include "Memory.h"

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

enum RepMode {
    REPNE, // Prefix 0xf2
    REPE, // Prefix 0xf3
};

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
        auto int_cs = mem->read<uint16_t>(VEC_INT + 2);
        auto int_ip = mem->read<uint16_t>(VEC_INT + 0);

        return registers->get(CS) == int_cs &&
            registers->get(IP) == int_ip;
    }

    void reset();
private:
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
    void pushff();
    void push50_57();
    void pushsr();
    void pop8f();
    void pop58_5f();
    void popsr();
    void xchg86();
    void xchg87();
    void xchg90_97();
    void ine4();
    void ine5();
    void inec();
    void ined();
    void oute6();
    void oute7();
    void outee();
    void outef();
    void xlatd7();
    void lea8d();
    void ldsc5();
    void lesc4();
    void lahf9f();
    void sahf9e();
    void pushf9c();
    void popf9d();
    void add_adc_sub_sbb_cmp_xor_or_and_80();
    void add_adc_sub_sbb_cmp_xor_or_and_81();
    void add_adc_sub_sbb_cmp_82();
    void add_adc_sub_sbb_cmp_83();
    void add00();
    void add01();
    void add02();
    void add03();
    void add04();
    void add05();
    void and20();
    void and21();
    void and22();
    void and23();
    void and24();
    void and25();
    void xor30();
    void xor31();
    void xor32();
    void xor33();
    void xor34();
    void xor35();
    void or08();
    void or09();
    void or0a();
    void or0b();
    void or0c();
    void or0d();
    void adc10();
    void adc11();
    void adc12();
    void adc13();
    void adc14();
    void adc15();
    void sub28();
    void sub29();
    void sub2a();
    void sub2b();
    void sub2c();
    void sub2d();
    void sbb18();
    void sbb19();
    void sbb1a();
    void sbb1b();
    void sbb1c();
    void sbb1d();
    void inc_dec_fe();
    void incff();
    void inc40_47();
    void decff();
    void dec48_4f();
    void aaa37();
    void daa27();
    void aas3f();
    void das2f();
    void aamd4();
    void negf6();
    void negf7();
    void notf6();
    void notf7();
    void cmp38();
    void cmp39();
    void cmp3a();
    void cmp3b();
    void cmp3c();
    void cmp3d();
    void mulf6();
    void mulf7();
    void imulf6();
    void imulf7();
    void intcc();
    void intcd();
    void intoce();
    void cbw98();
    void cwd99();
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
    void jns79();
    void jne75();
    void jnl7d();
    void jnle7f();
    void jnb73();
    void jnbe77();
    void jnp7b();
    void jno71();
    void jcxze3();
    void calle8();
    void callff_intra();
    void callff_inter();
    void call9a();
    void retc3();
    void retc2();
    void retcb();
    void retca();
    void iretcf();
    void clcf8();
    void cmcf5();
    void stcf9();
    void cldfc();
    void stdfd();
    void clifa();
    void stifb();
    void loope2();
    void loopee1();
    void loopnze0();
    void scasbae();
    void scasbaf();
    void movsba4();
    void movswa5();
    void cmpsba6();
    void cmpswa7();
    void lodsbac();
    void lodswad();
    void stosbaa();
    void stoswab();
    void hltf4();
    void wait9b();
    void escd8();
    void aadd5();
    void shiftd0();
    void shiftd1();
    void shiftd2();
    void shiftd3();
    void shld0();
    void shld1();
    void shld2();
    void shld3();
    void shrd0();
    void shrd1();
    void shrd2();
    void shrd3();
    void sard0();
    void sard1();
    void sard2();
    void sard3();
    void rold0();
    void rold1();
    void rold2();
    void rold3();
    void rcld0();
    void rcld1();
    void rcld2();
    void rcld3();
    void rcrd0();
    void rcrd1();
    void rcrd2();
    void rcrd3();
    void rord0();
    void rord1();
    void rord2();
    void rord3();
    void test84();
    void test85();
    void testa8();
    void testa9();
    void testf6();
    void testf7();

    uint8_t fetch_byte();
    template <typename T>
    std::pair<uint16_t, T> do_mul(int32_t v1, int32_t v2);
    template <typename T>
    std::pair<uint16_t, T> do_add(uint16_t v1, uint16_t v2,
                                  uint16_t carry_in=0);
    template <typename T>
    std::pair<uint16_t, T> do_sub(uint16_t v1, uint16_t v2,
                                  uint16_t carry_in=0);
    template <typename T>
    std::pair<uint16_t, T> do_xor(uint16_t v1, uint16_t v2);
    template <typename T>
    std::pair<uint16_t, T> do_or(uint16_t v1, uint16_t v2);
    template <typename T>
    std::pair<uint16_t, T> do_and(uint16_t v1, uint16_t v2);
    void push_inc_jmp_call_ff();
    void neg_mul_not_test_f6();
    void neg_mul_not_test_f7();
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
    GPR get_segment(bool is_stack_reference);
    void set_override_segment(GPR segment);

    Memory *mem;
    Memory *io;
    RegisterFile *registers;
    size_t instr_length = 0;
    std::unique_ptr<ModRMDecoder> modrm_decoder;
    uint8_t opcode;
    bool jump_taken;

    bool default_segment_overriden;
    GPR override_segment;

    RepMode rep_mode;
    bool has_rep_prefix;

    void do_rep(std::function<void()> primitive,
                std::function<bool()> should_terminate);
    bool string_rep_complete();
};

void EmulatorPimpl::do_rep(std::function<void()> primitive,
                           std::function<bool()> should_terminate)
{
    if (!has_rep_prefix) {
        primitive();
        return;
    }

    while (registers->get(CX) != 0) {
        primitive();
        registers->set(CX, registers->get(CX) - 1);
        if (should_terminate())
            break;
    }
}

bool EmulatorPimpl::string_rep_complete()
{
    if (rep_mode == REPE && !registers->get_flag(ZF))
        return true;
    if (rep_mode == REPNE && registers->get_flag(ZF))
        return true;
    return false;
}

EmulatorPimpl::EmulatorPimpl(RegisterFile *registers)
    : registers(registers)
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
}

size_t EmulatorPimpl::emulate()
{
    instr_length = 0;
    bool processing_prefixes;
    default_segment_overriden = false;
    has_rep_prefix = false;
    modrm_decoder->clear();
    jump_taken = false;

    do {
        processing_prefixes = false;
        opcode = fetch_byte();
        switch (opcode) {
        case 0x06: // fallthrough
        case 0x0e: // fallthrough
        case 0x16: // fallthrough
        case 0x1e: pushsr(); break;
        case 0x07: // fallthrough
        case 0x0f: // fallthrough
        case 0x17: // fallthrough
        case 0x1f: popsr(); break;

        case 0x00: add00(); break;
        case 0x01: add01(); break;
        case 0x02: add02(); break;
        case 0x03: add03(); break;
        case 0x04: add04(); break;
        case 0x05: add05(); break;
        case 0x08: or08(); break;
        case 0x09: or09(); break;
        case 0x0a: or0a(); break;
        case 0x0b: or0b(); break;
        case 0x0c: or0c(); break;
        case 0x0d: or0d(); break;
        case 0x10: adc10(); break;
        case 0x11: adc11(); break;
        case 0x12: adc12(); break;
        case 0x13: adc13(); break;
        case 0x14: adc14(); break;
        case 0x15: adc15(); break;
        case 0x18: sbb18(); break;
        case 0x19: sbb19(); break;
        case 0x1a: sbb1a(); break;
        case 0x1b: sbb1b(); break;
        case 0x1c: sbb1c(); break;
        case 0x1d: sbb1d(); break;
        case 0x20: and20(); break;
        case 0x21: and21(); break;
        case 0x22: and22(); break;
        case 0x23: and23(); break;
        case 0x24: and24(); break;
        case 0x25: and25(); break;
        case 0x27: daa27(); break;
        case 0x28: sub28(); break;
        case 0x29: sub29(); break;
        case 0x2a: sub2a(); break;
        case 0x2b: sub2b(); break;
        case 0x2c: sub2c(); break;
        case 0x2d: sub2d(); break;
        case 0x2f: das2f(); break;
        case 0x30: xor30(); break;
        case 0x31: xor31(); break;
        case 0x32: xor32(); break;
        case 0x33: xor33(); break;
        case 0x34: xor34(); break;
        case 0x35: xor35(); break;
        case 0x37: aaa37(); break;
        case 0x38: cmp38(); break;
        case 0x39: cmp39(); break;
        case 0x3a: cmp3a(); break;
        case 0x3b: cmp3b(); break;
        case 0x3c: cmp3c(); break;
        case 0x3d: cmp3d(); break;
        case 0x3f: aas3f(); break;
        case 0x40 ... 0x47: inc40_47(); break;
        case 0x48 ... 0x4f: dec48_4f(); break;
        case 0x50 ... 0x57: push50_57(); break;
        case 0x58 ... 0x5f: pop58_5f(); break;
        case 0x60: jo70(); break;
        case 0x61: jno71(); break;
        case 0x62: jb72(); break;
        case 0x63: jnb73(); break;
        case 0x64: je74(); break;
        case 0x65: jne75(); break;
        case 0x66: jbe76(); break;
        case 0x67: jnbe77(); break;
        case 0x68: js78(); break;
        case 0x69: jns79(); break;
        case 0x6a: jp7a(); break;
        case 0x6b: jnp7b(); break;
        case 0x6c: jl7c(); break;
        case 0x6d: jnl7d(); break;
        case 0x6e: jle7e(); break;
        case 0x6f: jnle7f(); break;
        case 0x70: jo70(); break;
        case 0x71: jno71(); break;
        case 0x72: jb72(); break;
        case 0x73: jnb73(); break;
        case 0x74: je74(); break;
        case 0x75: jne75(); break;
        case 0x76: jbe76(); break;
        case 0x77: jnbe77(); break;
        case 0x78: js78(); break;
        case 0x79: jns79(); break;
        case 0x7a: jp7a(); break;
        case 0x7b: jnp7b(); break;
        case 0x7c: jl7c(); break;
        case 0x7d: jnl7d(); break;
        case 0x7e: jle7e(); break;
        case 0x7f: jnle7f(); break;
        case 0x80: add_adc_sub_sbb_cmp_xor_or_and_80(); break;
        case 0x81: add_adc_sub_sbb_cmp_xor_or_and_81(); break;
        case 0x82: add_adc_sub_sbb_cmp_82(); break;
        case 0x83: add_adc_sub_sbb_cmp_83(); break;
        case 0x84: test84(); break;
        case 0x85: test85(); break;
        case 0x86: xchg86(); break;
        case 0x87: xchg87(); break;
        case 0x88: mov88(); break;
        case 0x89: mov89(); break;
        case 0x8a: mov8a(); break;
        case 0x8b: mov8b(); break;
        case 0x8c: mov8c(); break;
        case 0x8d: lea8d(); break;
        case 0x8e: mov8e(); break;
        case 0x8f: pop8f(); break;
        case 0x90 ... 0x97: xchg90_97(); break;
        case 0x98: cbw98(); break;
        case 0x99: cwd99(); break;
        case 0x9a: call9a(); break;
        case 0x9b: wait9b(); break;
        case 0x9c: pushf9c(); break;
        case 0x9d: popf9d(); break;
        case 0x9e: sahf9e(); break;
        case 0x9f: lahf9f(); break;
        case 0xa0: mova0(); break;
        case 0xa1: mova1(); break;
        case 0xa2: mova2(); break;
        case 0xa3: mova3(); break;
        case 0xa4: movsba4(); break;
        case 0xa5: movswa5(); break;
        case 0xa6: cmpsba6(); break;
        case 0xa7: cmpswa7(); break;
        case 0xa8: testa8(); break;
        case 0xa9: testa9(); break;
        case 0xaa: stosbaa(); break;
        case 0xab: stoswab(); break;
        case 0xac: lodsbac(); break;
        case 0xad: lodswad(); break;
        case 0xae: scasbae(); break;
        case 0xaf: scasbaf(); break;
        case 0xb0 ... 0xb7: movb0_b7(); break;
        case 0xb8 ... 0xbf: movb8_bf(); break;
        case 0xc2: retc2(); break;
        case 0xc3: retc3(); break;
        case 0xc4: lesc4(); break;
        case 0xc5: ldsc5(); break;
        case 0xc6: movc6(); break;
        case 0xc7: movc7(); break;
        case 0xca: retca(); break;
        case 0xcb: retcb(); break;
        case 0xcc: intcc(); break;
        case 0xcd: intcd(); break;
        case 0xce: intoce(); break;
        case 0xcf: iretcf(); break;
        case 0xd0: shiftd0(); break;
        case 0xd1: shiftd1(); break;
        case 0xd2: shiftd2(); break;
        case 0xd3: shiftd3(); break;
        case 0xd4: aamd4(); break;
        case 0xd5: aadd5(); break;
        case 0xd7: xlatd7(); break;
        case 0xd8 ... 0xdf: escd8(); break;
        case 0xe0: loopnze0(); break;
        case 0xe1: loopee1(); break;
        case 0xe2: loope2(); break;
        case 0xe3: jcxze3(); break;
        case 0xe4: ine4(); break;
        case 0xe5: ine5(); break;
        case 0xe6: oute6(); break;
        case 0xe7: oute7(); break;
        case 0xe8: calle8(); break;
        case 0xe9: jmpe9(); break;
        case 0xea: jmpea(); break;
        case 0xeb: jmpeb(); break;
        case 0xec: inec(); break;
        case 0xed: ined(); break;
        case 0xee: outee(); break;
        case 0xef: outef(); break;
        case 0xf4: hltf4(); break;
        case 0xf5: cmcf5(); break;
        case 0xf6: neg_mul_not_test_div_f6(); break;
        case 0xf7: neg_mul_not_test_div_f7(); break;
        case 0xf8: clcf8(); break;
        case 0xf9: stcf9(); break;
        case 0xfa: clifa(); break;
        case 0xfb: stifb(); break;
        case 0xfc: cldfc(); break;
        case 0xfd: stdfd(); break;
        case 0xfe: inc_dec_fe(); break;
        case 0xff: push_inc_jmp_call_ff(); break;
        case 0xf0: // lock
            processing_prefixes = true;
            break;
        case 0x26:
            set_override_segment(ES);
            processing_prefixes = true;
            break;
        case 0x2e:
            set_override_segment(CS);
            processing_prefixes = true;
            break;
        case 0x36:
            set_override_segment(SS);
            processing_prefixes = true;
            break;
        case 0x3e:
            set_override_segment(DS);
            processing_prefixes = true;
            break;
        case 0xf2:
            rep_mode = REPNE;
            has_rep_prefix = true;
            processing_prefixes = true;
            break;
        case 0xf3:
            rep_mode = REPE;
            has_rep_prefix = true;
            processing_prefixes = true;
            break;
        default:
            std::cerr << "warning: unknown opcode 0x" << std::hex <<
                (unsigned)opcode << " at " << (unsigned)registers->get(CS) <<
                ":" << (unsigned)registers->get(IP) << std::endl;
        }
    } while (processing_prefixes);

    if (!jump_taken)
        registers->set(IP, registers->get(IP) + instr_length);

    return instr_length;
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
    else
        std::cerr << "warning: invalid reg " << std::hex <<
            (unsigned)modrm_decoder->raw_reg() <<
            " for opcode 0x" << (unsigned)opcode << std::endl;
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
    bool af = !!(alu_op(v1 & 0xf, v2 & 0xf, carry) & (1 << 4));

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
std::pair<uint16_t, T> EmulatorPimpl::do_xor(uint16_t v1, uint16_t v2)
{
    return do_alu<T>(v1, v2, 0,
        [](uint32_t a, uint32_t b, uint32_t __attribute__((unused)) c) -> uint32_t {
            return a ^ b;
        });
}

template <typename T>
std::pair<uint16_t, T> EmulatorPimpl::do_or(uint16_t v1, uint16_t v2)
{
    return do_alu<T>(v1, v2, 0,
        [](uint32_t a, uint32_t b, uint32_t __attribute__((unused)) c) -> uint32_t {
            return a | b;
        });
}

template <typename T>
std::pair<uint16_t, T> EmulatorPimpl::do_and(uint16_t v1, uint16_t v2)
{
    return do_alu<T>(v1, v2, 0,
        [](uint32_t a, uint32_t b, uint32_t __attribute__((unused)) c) -> uint32_t {
            return a & b;
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

void EmulatorPimpl::add_adc_sub_sbb_cmp_xor_or_and_80()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    uint8_t v1 = read_data<uint8_t>();
    uint8_t v2 = fetch_byte();
    bool carry_in = modrm_decoder->raw_reg() == 2 || modrm_decoder->raw_reg() == 3 ?
        !!(registers->get_flags() & CF) : 0;

    uint8_t result;
    uint16_t flags;
    uint16_t update_mask = OF | SF | ZF | CF | PF | AF;
    if (modrm_decoder->raw_reg() == 0 ||
        modrm_decoder->raw_reg() == 2)
        std::tie(flags, result) = do_add<uint8_t>(v1, v2, carry_in);
    else if (modrm_decoder->raw_reg() == 6) {
        std::tie(flags, result) = do_xor<uint8_t>(v1, v2);
        flags &= ~(CF | OF);
        update_mask &= ~AF;
    } else if (modrm_decoder->raw_reg() == 1) {
        std::tie(flags, result) = do_or<uint8_t>(v1, v2);
        flags &= ~(CF | OF);
        update_mask &= ~AF;
    } else if (modrm_decoder->raw_reg() == 4) {
        std::tie(flags, result) = do_and<uint8_t>(v1, v2);
        flags &= ~(CF | OF);
        update_mask &= ~AF;
    } else
        std::tie(flags, result) = do_sub<uint8_t>(v1, v2, carry_in);

    registers->set_flags(flags, update_mask);
    // cmp doesn't write the result
    if (modrm_decoder->raw_reg() != 7)
        write_data<uint8_t>(result & 0xff);
}

void EmulatorPimpl::add_adc_sub_sbb_cmp_82()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

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

    registers->set_flags(flags, OF | SF | ZF | CF | PF | AF);
    // cmp doesn't write the result
    if (modrm_decoder->raw_reg() != 7)
        write_data<uint8_t>(result & 0xff);
}

// add r/m, immediate, 16-bit
void EmulatorPimpl::add_adc_sub_sbb_cmp_xor_or_and_81()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    uint16_t v1 = read_data<uint16_t>();
    uint16_t v2 = fetch_16bit();
    bool carry_in = modrm_decoder->raw_reg() == 2 || modrm_decoder->raw_reg() == 3 ?
        !!(registers->get_flags() & CF) : 0;

    uint16_t result;
    uint16_t flags;
    uint16_t update_mask = OF | SF | ZF | CF | PF | AF;
    if (modrm_decoder->raw_reg() == 0 ||
        modrm_decoder->raw_reg() == 2)
        std::tie(flags, result) = do_add<uint16_t>(v1, v2, carry_in);
    else if (modrm_decoder->raw_reg() == 6) {
        std::tie(flags, result) = do_xor<uint16_t>(v1, v2);
        flags &= ~(CF | OF);
        update_mask &= ~AF;
    } else if (modrm_decoder->raw_reg() == 1) {
        std::tie(flags, result) = do_or<uint16_t>(v1, v2);
        flags &= ~(CF | OF);
        update_mask &= ~AF;
    } else if (modrm_decoder->raw_reg() == 4) {
        std::tie(flags, result) = do_and<uint16_t>(v1, v2);
        flags &= ~(CF | OF);
        update_mask &= ~AF;
    } else
        std::tie(flags, result) = do_sub<uint16_t>(v1, v2, carry_in);

    registers->set_flags(flags, update_mask);
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
        modrm_decoder->raw_reg() != 7) {
        std::cerr << "warning: invalid reg " << std::hex <<
            (unsigned)modrm_decoder->raw_reg() <<
            " for opcode 0x" << (unsigned)opcode << std::endl;
        return;
    }

    uint16_t v1 = read_data<uint16_t>();
    int16_t immed = sign_extend<int16_t, uint8_t>(fetch_byte());
    bool carry_in = modrm_decoder->raw_reg() == 2 || modrm_decoder->raw_reg() == 3 ?
        !!(registers->get_flags() & CF) : 0;

    uint16_t result;
    uint16_t flags;
    uint16_t update_mask = OF | SF | ZF | CF | PF | AF;
    if (modrm_decoder->raw_reg() == 0 ||
        modrm_decoder->raw_reg() == 2) {
        std::tie(flags, result) = do_add<uint16_t>(v1, immed, carry_in);
    } else if (modrm_decoder->raw_reg() == 4) {
        std::tie(flags, result) = do_and<uint16_t>(v1, immed);
        flags &= ~(CF | OF);
        update_mask &= ~AF;
    } else if (modrm_decoder->raw_reg() == 1) {
        std::tie(flags, result) = do_or<uint16_t>(v1, immed);
        flags &= ~(CF | OF);
        update_mask &= ~AF;
    } else {
        std::tie(flags, result) = do_sub<uint16_t>(v1, immed, carry_in);
    }

    registers->set_flags(flags, update_mask);
    // cmp doesn't write the result
    if (modrm_decoder->raw_reg() != 7)
        write_data<uint16_t>(result & 0xffff);
}

uint8_t EmulatorPimpl::fetch_byte()
{
    return mem->read<uint8_t>(get_phys_addr(registers->get(CS),
                                            registers->get(IP) + instr_length++));
}

void EmulatorPimpl::neg_mul_not_test_f6()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() == 0x2)
        notf6();
    else if (modrm_decoder->raw_reg() == 0x3)
        negf6();
    else if (modrm_decoder->raw_reg() == 0x4)
        mulf6();
    else if (modrm_decoder->raw_reg() == 0x5)
        imulf6();
    else if (modrm_decoder->raw_reg() == 0x0)
        testf6();
    else
        std::cerr << "warning: invalid reg " << std::hex <<
            (unsigned)modrm_decoder->raw_reg() <<
            " for opcode 0x" << (unsigned)opcode << std::endl;
}

void EmulatorPimpl::neg_mul_not_test_f7()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() == 0x2)
        notf7();
    else if (modrm_decoder->raw_reg() == 0x3)
        negf7();
    else if (modrm_decoder->raw_reg() == 0x4)
        mulf7();
    else if (modrm_decoder->raw_reg() == 0x5)
        imulf7();
    else if (modrm_decoder->raw_reg() == 0x0)
        testf7();
    else
        std::cerr << "warning: invalid reg " << std::hex <<
            (unsigned)modrm_decoder->raw_reg() <<
            " for opcode 0x" << (unsigned)opcode << std::endl;
}

void EmulatorPimpl::shiftd0()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() == 4)
        shld0();
    else if (modrm_decoder->raw_reg() == 5)
        shrd0();
    else if (modrm_decoder->raw_reg() == 7)
        sard0();
    else if (modrm_decoder->raw_reg() == 0)
        rold0();
    else if (modrm_decoder->raw_reg() == 2)
        rcld0();
    else if (modrm_decoder->raw_reg() == 3)
        rcrd0();
    else if (modrm_decoder->raw_reg() == 1)
        rord0();
    else
        std::cerr << "warning: invalid reg " << std::hex <<
            (unsigned)modrm_decoder->raw_reg() <<
            " for opcode 0x" << (unsigned)opcode << std::endl;
}

void EmulatorPimpl::shiftd1()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() == 4)
        shld1();
    else if (modrm_decoder->raw_reg() == 5)
        shrd1();
    else if (modrm_decoder->raw_reg() == 7)
        sard1();
    else if (modrm_decoder->raw_reg() == 0)
        rold1();
    else if (modrm_decoder->raw_reg() == 2)
        rcld1();
    else if (modrm_decoder->raw_reg() == 3)
        rcrd1();
    else if (modrm_decoder->raw_reg() == 1)
        rord1();
    else
        std::cerr << "warning: invalid reg " << std::hex <<
            (unsigned)modrm_decoder->raw_reg() <<
            " for opcode 0x" << (unsigned)opcode << std::endl;
}

void EmulatorPimpl::shiftd2()
{
    modrm_decoder->set_width(OP_WIDTH_8);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() == 4)
        shld2();
    else if (modrm_decoder->raw_reg() == 5)
        shrd2();
    else if (modrm_decoder->raw_reg() == 7)
        sard2();
    else if (modrm_decoder->raw_reg() == 0)
        rold2();
    else if (modrm_decoder->raw_reg() == 2)
        rcld2();
    else if (modrm_decoder->raw_reg() == 1)
        rord2();
    else if (modrm_decoder->raw_reg() == 3)
        rcrd2();
    else
        std::cerr << "warning: invalid reg " << std::hex <<
            (unsigned)modrm_decoder->raw_reg() <<
            " for opcode 0x" << (unsigned)opcode << std::endl;
}

void EmulatorPimpl::shiftd3()
{
    modrm_decoder->set_width(OP_WIDTH_16);
    modrm_decoder->decode();

    if (modrm_decoder->raw_reg() == 4)
        shld3();
    else if (modrm_decoder->raw_reg() == 5)
        shrd3();
    else if (modrm_decoder->raw_reg() == 7)
        sard3();
    else if (modrm_decoder->raw_reg() == 0)
        rold3();
    else if (modrm_decoder->raw_reg() == 2)
        rcld3();
    else if (modrm_decoder->raw_reg() == 1)
        rord3();
    else if (modrm_decoder->raw_reg() == 3)
        rcrd3();
    else
        std::cerr << "warning: invalid reg " << std::hex <<
            (unsigned)modrm_decoder->raw_reg() <<
            " for opcode 0x" << (unsigned)opcode << std::endl;
}

#include "instructions/mov.cpp"
#include "instructions/push.cpp"
#include "instructions/pop.cpp"
#include "instructions/xchg.cpp"
#include "instructions/io.cpp"
#include "instructions/xlat.cpp"
#include "instructions/lea.cpp"
#include "instructions/lahf_sahf.cpp"
#include "instructions/add.cpp"
#include "instructions/xor.cpp"
#include "instructions/and.cpp"
#include "instructions/or.cpp"
#include "instructions/adc.cpp"
#include "instructions/sub.cpp"
#include "instructions/sbb.cpp"
#include "instructions/inc_dec.cpp"
#include "instructions/aaa.cpp"
#include "instructions/daa.cpp"
#include "instructions/aas.cpp"
#include "instructions/aam.cpp"
#include "instructions/das.cpp"
#include "instructions/aad.cpp"
#include "instructions/neg.cpp"
#include "instructions/cmp.cpp"
#include "instructions/mul.cpp"
#include "instructions/imul.cpp"
#include "instructions/int.cpp"
#include "instructions/cbw.cpp"
#include "instructions/cwd.cpp"
#include "instructions/jmp.cpp"
#include "instructions/call.cpp"
#include "instructions/ret.cpp"
#include "instructions/clc.cpp"
#include "instructions/cmc.cpp"
#include "instructions/stc.cpp"
#include "instructions/cld.cpp"
#include "instructions/std.cpp"
#include "instructions/cli.cpp"
#include "instructions/sti.cpp"
#include "instructions/loop.cpp"
#include "instructions/loope.cpp"
#include "instructions/loopnz.cpp"
#include "instructions/scas.cpp"
#include "instructions/movs.cpp"
#include "instructions/cmps.cpp"
#include "instructions/lods.cpp"
#include "instructions/stos.cpp"
#include "instructions/hlt.cpp"
#include "instructions/wait.cpp"
#include "instructions/esc.cpp"
#include "instructions/not.cpp"
#include "instructions/shl.cpp"
#include "instructions/shr.cpp"
#include "instructions/sar.cpp"
#include "instructions/rol.cpp"
#include "instructions/rcl.cpp"
#include "instructions/ror.cpp"
#include "instructions/rcr.cpp"
#include "instructions/test.cpp"

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
        auto segment = get_segment(stack);
        auto addr = get_phys_addr(registers->get(segment), ea);
        mem->write<T>(addr, val);
    }
}

template <typename T>
T EmulatorPimpl::read_data(bool stack)
{
    if (modrm_decoder->rm_type() == OP_MEM) {
        auto displacement = modrm_decoder->effective_address();
        auto segment = get_segment(stack);
        auto addr = get_phys_addr(registers->get(segment), displacement);

        return mem->read<T>(addr);
    } else {
        auto source = modrm_decoder->rm_reg();
        return registers->get(source);
    }
}

GPR EmulatorPimpl::get_segment(bool is_stack_reference)
{
    if (is_stack_reference)
        return SS;

    if (default_segment_overriden)
        return override_segment;

    return modrm_decoder->uses_bp_as_base() || is_stack_reference ? SS : DS;
}

void EmulatorPimpl::set_override_segment(GPR segment)
{
    default_segment_overriden = true;
    override_segment = segment;
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
