#ifndef __TESTMODRM_H__
#define __TESTMODRM_H__

#include <vector>

#include "RegisterFile.h"
#include "ModRM.h"

class ModRMDecoderTestBench {
public:
    virtual void set_instruction(const std::vector<uint8_t> instruction) = 0;
    virtual void decode() = 0;
    virtual OperandType get_rm_type() const = 0;
    virtual uint16_t get_effective_address() const = 0;
    virtual GPR get_register() const = 0;
    virtual GPR get_rm_register() const = 0;
    RegisterFile *get_registers()
    {
        return &regs;
    }
protected:
    RegisterFile regs;
};

#endif /* __TESTMODRM_H__ */
