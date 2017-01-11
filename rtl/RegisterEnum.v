`ifndef REGISTERENUM
`define REGISTERENUM

typedef enum bit [2:0] {
    AX,
    CX,
    DX,
    BX,
    SP,
    BP,
    SI,
    DI
} GPR16_t;

typedef enum bit [2:0] {
    AL,
    CL,
    DL,
    BL,
    AH,
    CH,
    DH,
    BH
} GPR8_t;

`endif
