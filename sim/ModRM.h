#ifndef __MODRM_H__
#define __MODRM_H__

#include <stdint.h>
#include <functional>
#include <functional>

#include "Fifo.h"
#include "RegisterFile.h"

enum Reg {
    MODRM_REG_AL = 0,
    MODRM_REG_AX = 0,
    MODRM_REG_CL = 1,
    MODRM_REG_CX = 1,
    MODRM_REG_DL = 2,
    MODRM_REG_DX = 2,
    MODRM_REG_BL = 3,
    MODRM_REG_BX = 3,
    MODRM_REG_AH = 4,
    MODRM_REG_SP = 4,
    MODRM_REG_CH = 5,
    MODRM_REG_BP = 5,
    MODRM_REG_DH = 6,
    MODRM_REG_SI = 6,
    MODRM_REG_BH = 7,
    MODRM_REG_DI = 7
};

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
    GPR rm_reg() const;
    uint16_t effective_address() const;
    OperandType rm_type() const;

private:
    std::function<uint8_t()> get_byte;
    const RegisterFile *registers;
    OperandWidth width;
    uint8_t modrm;
};

#endif /* __MODRM_H__ */
