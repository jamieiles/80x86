#ifndef __EMULATE_H__
#define __EMULATE_H__

#include <stddef.h>
#include <stdint.h>
#include <memory>

#include "ModRM.h"

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

private:
    uint8_t fetch_byte();
    void mov88();
    void mov89();
    template <typename T>
        void write_result(T val);

    Fifo<uint8_t> *instr_stream;
    Memory *mem;
    RegisterFile *registers;
    size_t instr_length = 0;
    std::unique_ptr<ModRMDecoder> modrm_decoder;
};

#endif /* __EMULATE_H__ */
