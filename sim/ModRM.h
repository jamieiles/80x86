#ifndef __MODRM_H__
#define __MODRM_H__

#include <stdint.h>
#include <functional>
#include <functional>

#include "Fifo.h"
#include "RegisterFile.h"

enum OperandType {
    OP_REG,
    OP_MEM,
};

enum OperandWidth {
    OP_WIDTH_8,
    OP_WIDTH_16
};

class ModRMDecoder {
public:
    ModRMDecoder(std::function<uint8_t()> get_byte,
                 const RegisterFile *registers);
    void set_width(OperandWidth width);
    void decode();
    GPR reg() const;
    int raw_reg() const;
    GPR rm_reg() const;
    uint16_t effective_address() const;
    OperandType rm_type() const;
    bool uses_bp_as_base() const;

private:
    std::function<uint8_t()> get_byte;
    const RegisterFile *registers;
    OperandWidth width;
    uint8_t modrm;
};

#endif /* __MODRM_H__ */
