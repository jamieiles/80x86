#ifndef __EMULATE_H__
#define __EMULATE_H__

#include <stddef.h>
#include <stdint.h>
#include <memory>

#include "Memory.h"
#include "ModRM.h"

static inline phys_addr get_phys_addr(uint16_t segment,
                                      uint16_t displacement)
{
    return (static_cast<uint32_t>(segment) << 4) + displacement;
}

template <typename T> class Fifo;
class Memory;
class RegisterFile;

class Emulator {
public:
    Emulator(RegisterFile *registers);

    size_t emulate();

    void set_instruction_stream(Fifo<uint8_t> *instr_stream)
    {
        this->instr_stream = instr_stream;
    }

    void set_memory(Memory *mem)
    {
        this->mem = mem;
    }

    void set_io(Memory *io)
    {
        this->io = io;
    }

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
    // add
    //
    template <typename T>
    T do_add(uint16_t v1, uint16_t v2);
    void add00();
    void add01();
    void add02();
    void add03();
    void add80();
    void add81();
    void add82();
    void add83();
    void add04();
    void add05();
    template <typename T>
        void write_data(T val, bool stack=false);
    template <typename T>
        T read_data(bool stack=false);
    uint16_t fetch_16bit();

    Fifo<uint8_t> *instr_stream;
    Memory *mem;
    Memory *io;
    RegisterFile *registers;
    size_t instr_length = 0;
    std::unique_ptr<ModRMDecoder> modrm_decoder;
    uint8_t opcode;
};

#endif /* __EMULATE_H__ */
