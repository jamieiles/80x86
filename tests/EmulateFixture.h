#ifndef __EMULATEFIXTURE_H__
#define __EMULATEFIXTURE_H__

#include <gtest/gtest.h>

#include "Emulate.h"
#include "Fifo.h"
#include "Memory.h"
#include "ModRM.h"
#include "RegisterFile.h"

class EmulateFixture : public ::testing::Test {
public:
    EmulateFixture()
        : instr_stream(16),
        emulator(&registers)
    {
        emulator.set_instruction_stream(&instr_stream);
        emulator.set_memory(&mem);
    }

    void set_instruction(const std::vector<uint8_t> &instr)
    {
        for (auto &b: instr)
            instr_stream.push(b);
    }
protected:
    Fifo<uint8_t> instr_stream;
    Memory mem;
    RegisterFile registers;
    Emulator emulator;
};

#endif /* __EMULATEFIXTURE_H__ */
