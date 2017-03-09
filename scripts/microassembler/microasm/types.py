from enum import Enum

class GPR(Enum):
    AX = 0
    CX = 1
    DX = 2
    BX = 3
    SP = 4
    BP = 5
    SI = 6
    DI = 7
    AL = 0
    CL = 1
    DL = 2
    BL = 3
    AH = 4
    CH = 5
    DH = 6
    BH = 7

class SR(Enum):
    ES = 0
    CS = 1
    SS = 2
    DS = 3

class ADriver(Enum):
    TEMP = 0
    RA = 1
    IP = 2
    Q = 3

class BDriver(Enum):
    RB = 0
    IMMEDIATE = 1
    SR = 2
    Q = 3

class QDriver(Enum):
    MAR = 0
    MDR = 1
    ALU = 2

class ALUOp(Enum):
    XOR = 0
    ADD = 1
    SELA = 2
    SELB = 3

class UpdateFlags(Enum):
    CF = 0
    PF = 1
    AF = 2
    ZF = 3
    SF = 4
    TF = 5
    IF = 6
    DF = 7
    OF = 8

class RDSelSource(Enum):
    MODRM_REG = 0
    MODRM_RM_REG = 1
    MICROCODE_RD_SEL = 2

class JumpType(Enum):
    UNCONDITIONAL = 0
    RM_REG_MEM = 1
    OPCODE = 2
    DISPATCH_REG = 3
