#ifndef __REGISTERFILE_H__
#define __REGISTERFILE_H__

#include <stdint.h>

enum GPR {
    AX,
    CX,
    DX,
    BX,
    SP,
    BP,
    SI,
    DI,
    IP,
    CS,
    DS,
    SS,
    ES,
    NUM_16BIT_REGS = ES,
    AL,
    CL,
    DL,
    BL,
    AH,
    CH,
    DH,
    BH,
    NUM_REGS
};

enum FlagBitPos {
    CF = 0,
    PF = 2,
    AF = 4,
    ZF = 6,
    SF = 7,
    TF = 8,
    IF = 9,
    DF = 10,
    OF = 11
};

class RegisterFile {
public:
    RegisterFile();
    virtual void reset();
    virtual void set(GPR regnum, uint16_t value);
    virtual uint16_t get(GPR regnum) const;
    virtual uint16_t get_flags() const;
    virtual void set_flags(uint16_t val);
private:
    uint16_t registers[NUM_16BIT_REGS];
    uint16_t flags;
};

#endif /* __REGISTERFILE_H__ */
