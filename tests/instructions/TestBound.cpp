#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

TEST_F(EmulateFixture, BoundInvalidOperand)
{
    write_mem16(VEC_INVALID_OPCODE + 2, 0x8000, CS); // CS
    write_mem16(VEC_INVALID_OPCODE + 0, 0x0100, CS); // IP

    write_reg(SP, 0x100);
    write_reg(CS, 0x7c00);
    write_reg(IP, 0x0001);

    set_instruction({ 0x62, 0xd8 });
    emulate();

    EXPECT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS);
    EXPECT_EQ(read_reg(CS), 0x8000);
    EXPECT_EQ(read_reg(IP), 0x0100);
    EXPECT_EQ(read_reg(SP), 0x0100 - 6);

    EXPECT_EQ(read_mem16(0x100 - 2, SS), FLAGS_STUCK_BITS);
    EXPECT_EQ(read_mem16(0x100 - 4, SS), 0x7c00);
    // Return to the following instruction
    EXPECT_EQ(read_mem16(0x100 - 6, SS), 0x0003);
}

struct BoundParams {
    int16_t lower;
    int16_t upper;
    int16_t v;
    bool trap_expected;
};

class BoundFixture : public EmulateFixture,
    public ::testing::WithParamInterface<BoundParams> {
};
TEST_P(BoundFixture, Bound)
{
    auto p = GetParam();

    write_mem16(VEC_BOUND + 2, 0x8000, CS); // CS
    write_mem16(VEC_BOUND + 0, 0x0100, CS); // IP

    write_reg(SP, 0x100);
    write_reg(CS, 0x7c00);
    write_reg(IP, 0x0001);

    // bound ax, [bx]
    set_instruction({ 0x62, 0x07 });
    write_mem16(0x1000, p.lower);
    write_mem16(0x1002, p.upper);
    write_reg(BX, 0x1000);
    write_reg(AX, p.v);

    emulate();

    if (p.trap_expected) {
        EXPECT_EQ(read_reg(CS), 0x8000);
        EXPECT_EQ(read_reg(IP), 0x0100);
        EXPECT_EQ(read_reg(SP), 0x0100 - 6);

        EXPECT_EQ(read_mem16(0x100 - 2, SS), FLAGS_STUCK_BITS);
        EXPECT_EQ(read_mem16(0x100 - 4, SS), 0x7c00);
        // Return to the following instruction
        EXPECT_EQ(read_mem16(0x100 - 6, SS), 0x0003);
        EXPECT_EQ(read_flags(), FLAGS_STUCK_BITS);
    } else {
        EXPECT_EQ(read_reg(CS), 0x7c00);
        EXPECT_EQ(read_reg(IP), 0x0003);
        EXPECT_EQ(read_flags(), FLAGS_STUCK_BITS);
    }
}
INSTANTIATE_TEST_CASE_P(Bound, BoundFixture,
    ::testing::Values(
        BoundParams{ 100, 200, 150, false },
        BoundParams{ 100, 200, 100, false },
        BoundParams{ 100, 200, 200, false },
        BoundParams{ 100, 200, 99, true },
        BoundParams{ 100, 200, 201, true },
        BoundParams{ -8, -4, -10, true },
        BoundParams{ -8, -4, -2, true },
        BoundParams{ -8, -4, -6, false },
        BoundParams{ -8, -4, -6, false }
    ));
