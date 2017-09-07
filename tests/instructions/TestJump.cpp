#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

TEST_F(EmulateFixture, JmpDirectIntra)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    set_instruction({0xe9, 0x10, 0x20});

    emulate();

    ASSERT_EQ(0x2000, read_reg(CS));
    ASSERT_EQ(0x2043, read_reg(IP));
}

TEST_F(EmulateFixture, JmpDirectIntraShort)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0100);
    set_instruction({0xeb, 0x80});

    emulate();

    ASSERT_EQ(0x2000, read_reg(CS));
    ASSERT_EQ(0x0082, read_reg(IP));
}

TEST_F(EmulateFixture, JmpIndirectIntraReg)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    write_reg(AX, 0x0100);
    // jmp ax
    set_instruction({0xff, 0xe0});

    emulate();

    ASSERT_EQ(0x2000, read_reg(CS));
    ASSERT_EQ(0x0100, read_reg(IP));
}

TEST_F(EmulateFixture, JmpIndirectIntraMem)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);

    write_reg(BX, 0x0800);
    write_mem16(0x0800, 0x0100);
    // jmp [bx]
    set_instruction({0xff, 0x27});

    emulate();

    ASSERT_EQ(0x2000, read_reg(CS));
    ASSERT_EQ(0x0100, read_reg(IP));
}

TEST_F(EmulateFixture, DirectInter)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);

    // jmp 0x8000:0x1234
    set_instruction({0xea, 0x34, 0x12, 0x00, 0x80});

    emulate();

    ASSERT_EQ(0x8000, read_reg(CS));
    ASSERT_EQ(0x1234, read_reg(IP));
}

TEST_F(EmulateFixture, JmpIndirectInterReg)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);
    // jmp far ax (invalid encoding)
    set_instruction({0xff, 0xe8});

    emulate();

    ASSERT_EQ(0x2000, read_reg(CS));
    ASSERT_EQ(0x0032, read_reg(IP));

    ASSERT_FALSE(instruction_had_side_effects());
}

TEST_F(EmulateFixture, JmpIndirectInterMem)
{
    write_reg(CS, 0x2000);
    write_reg(IP, 0x0030);

    write_reg(BX, 0x0800);
    write_mem16(0x0800, 0x5678);
    write_mem16(0x0802, 0x4000);
    // jmp far [bx]
    set_instruction({0xff, 0x2f});

    emulate();

    ASSERT_EQ(0x4000, read_reg(CS));
    ASSERT_EQ(0x5678, read_reg(IP));
}

struct JmpTest {
    const char *name;
    uint8_t opcode;
    uint16_t flags;
    bool taken;
};

class JmpFixture : public EmulateFixture,
                   public ::testing::WithParamInterface<JmpTest>
{
};

TEST_P(JmpFixture, TakenNotTaken)
{
    auto old_ip = read_reg(IP);

    SCOPED_TRACE(GetParam().name + std::string(" [") +
                 flags_to_string(GetParam().flags) + std::string("]") +
                 (GetParam().taken ? " (taken)" : " (not taken)"));

    write_flags(GetParam().flags);
    set_instruction({GetParam().opcode, 0x0e});

    emulate();

    if (GetParam().taken)
        ASSERT_EQ(read_reg(IP), old_ip + 0x10);
    else
        ASSERT_EQ(read_reg(IP), old_ip + 0x02);
}
INSTANTIATE_TEST_CASE_P(
    JmpConditional,
    JmpFixture,
    ::testing::Values(JmpTest{"je/jz", 0x74, ZF, true},
                      JmpTest{"je/jz", 0x74, 0, false},

                      JmpTest{"jl/jnge", 0x7c, SF, true},
                      JmpTest{"jl/jnge", 0x7c, OF, true},
                      JmpTest{"jl/jnge", 0x7c, OF | SF, false},
                      JmpTest{"jl/jnge", 0x7c, 0, false},

                      JmpTest{"jle/jng", 0x7e, SF, true},
                      JmpTest{"jle/jng", 0x7e, OF, true},
                      JmpTest{"jle/jng", 0x7e, OF | SF, false},
                      JmpTest{"jle/jng", 0x7e, ZF | OF | SF, true},
                      JmpTest{"jle/jng", 0x7e, ZF, true},
                      JmpTest{"jle/jng", 0x7e, 0, false},

                      JmpTest{"jb/jnae", 0x72, 0, false},
                      JmpTest{"jb/jnae", 0x72, CF, true},

                      JmpTest{"jbe/jna", 0x76, 0, false},
                      JmpTest{"jbe/jna", 0x76, CF, true},
                      JmpTest{"jbe/jna", 0x76, ZF, true},
                      JmpTest{"jbe/jna", 0x76, CF | ZF, true},

                      JmpTest{"jp/jpe", 0x7a, 0, false},
                      JmpTest{"jp/jpe", 0x7a, PF, true},

                      JmpTest{"jo", 0x70, 0, false},
                      JmpTest{"jo", 0x70, OF, true},

                      JmpTest{"js", 0x78, 0, false},
                      JmpTest{"js", 0x78, SF, true},

                      JmpTest{"jne/jnz", 0x75, 0, true},
                      JmpTest{"jne/jnz", 0x75, ZF, false},

                      JmpTest{"jnl/jge", 0x7d, 0, true},
                      JmpTest{"jnl/jge", 0x7d, SF | OF, true},
                      JmpTest{"jnl/jge", 0x7d, OF, false},
                      JmpTest{"jnl/jge", 0x7d, SF, false},

                      JmpTest{"jnle/jg", 0x7f, ZF, false},
                      JmpTest{"jnle/jg", 0x7f, 0, true},
                      JmpTest{"jnle/jg", 0x7f, SF | OF, true},
                      JmpTest{"jnle/jg", 0x7f, SF, false},
                      JmpTest{"jnle/jg", 0x7f, OF, false},

                      JmpTest{"jnb/jae", 0x73, 0, true},
                      JmpTest{"jnb/jae", 0x73, CF, false},

                      JmpTest{"jnbe/ja", 0x77, 0, true},
                      JmpTest{"jnbe/ja", 0x77, CF, false},
                      JmpTest{"jnbe/ja", 0x77, ZF, false},
                      JmpTest{"jnbe/ja", 0x77, CF | ZF, false},

                      JmpTest{"jnp/jpo", 0x7b, 0, true},
                      JmpTest{"jnp/jpo", 0x7b, PF, false},

                      JmpTest{"jno", 0x71, 0, true},
                      JmpTest{"jno", 0x71, OF, false},

                      JmpTest{"jns", 0x79, SF, false},
                      JmpTest{"jns", 0x79, 0, true}));

TEST_F(EmulateFixture, JcxzNotTaken)
{
    write_reg(CX, 1);
    write_reg(IP, 0x0030);
    set_instruction({0xe3, 0x80});

    emulate();

    ASSERT_EQ(read_reg(IP), 0x0032);
}

TEST_F(EmulateFixture, JcxzTaken)
{
    write_reg(CX, 0);
    write_reg(IP, 0x0030);
    set_instruction({0xe3, 0x10});

    emulate();

    ASSERT_EQ(read_reg(IP), 0x0042);
}
