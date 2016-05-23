#ifndef __MODRM_H__
#define __MODRM_H__

#include <stdint.h>
#include <functional>
#include <functional>

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
    void clear();
    void set_width(OperandWidth width);
    void decode();
    GPR reg() const;
    int raw_reg() const;
    GPR rm_reg() const;
    uint16_t effective_address();
    OperandType rm_type() const;
    bool uses_bp_as_base() const;

private:
    uint16_t calculate_effective_address();
    uint16_t mod00();
    uint16_t mod01();
    uint16_t mod10();
    std::function<uint8_t()> get_byte;
    const RegisterFile *registers;
    OperandWidth width;
    uint8_t modrm;
    uint16_t cached_address;
    bool addr_is_cached;
    bool is_decoded;
};

#endif /* __MODRM_H__ */
