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
    RA = 0
    IP = 1
    MAR = 2
    MDR = 3

class BDriver(Enum):
    RB = 0
    IMMEDIATE = 1
    SR = 2
    TEMP = 3

class ALUOp(Enum):
    SELA = 0
    SELB = 1
    ADD = 2
    ADC = 3
    AND = 4
    XOR = 5
    OR = 6
    SUB = 7
    SUBREV = 8
    SBB = 9
    SBBREV = 10
    GETFLAGS = 11
    SETFLAGSB = 12
    SETFLAGSA = 13
    CMC = 14
    SHR = 15
    SHL = 16
    SAR = 17
    ROR = 18
    ROL = 19
    RCL = 20
    RCR = 21
    NOT = 22
    NEXT = 23
    AAA = 24
    AAS = 25
    DAA = 26
    DAS = 27
    MUL = 28
    IMUL = 29
    DIV = 30
    IDIV = 31
    EXTEND = 32
    BOUNDL = 33
    BOUNDH = 34

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
    HAS_NO_REP_PREFIX = 4
    ZERO = 5
    REP_NOT_COMPLETE = 6
    JUMP_TAKEN = 7
    RB_ZERO = 8

class MARWrSel(Enum):
    EA = 0
    Q = 1

class RegWrSource(Enum):
    Q = 0
    QUOTIENT = 1
    REMAINDER = 2

class TEMPWrSel(Enum):
    Q_LOW = 0
    Q_HIGH = 1

class PrefixType(Enum):
    NONE = 0
    SEGMENT_OVERRIDE = 1
    REPNE = 2
    REPE = 3
