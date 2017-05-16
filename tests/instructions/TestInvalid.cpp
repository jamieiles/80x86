#include <vector>
#include <gtest/gtest.h>

#include "EmulateFixture.h"
#include "Flags.h"

class InvalidOpcodeFixture : public EmulateFixture,
    public ::testing::WithParamInterface<std::vector<uint8_t>> {
};
TEST_P(InvalidOpcodeFixture, InvalidOpcode)
{
    write_mem16(VEC_INVALID_OPCODE + 2, 0x8000, CS); // CS
    write_mem16(VEC_INVALID_OPCODE + 0, 0x0100, CS); // IP

    write_reg(SP, 0x100);
    write_reg(CS, 0x7c00);
    write_reg(IP, 0x0001);
    write_flags(IF);

    set_instruction(GetParam());

    emulate();

    EXPECT_PRED_FORMAT2(AssertFlagsEqual, read_flags(),
                        FLAGS_STUCK_BITS);
    EXPECT_EQ(read_reg(CS), 0x8000);
    EXPECT_EQ(read_reg(IP), 0x0100);
    EXPECT_EQ(read_reg(SP), 0x0100 - 6);

    EXPECT_EQ(read_mem16(0x100 - 2, SS), FLAGS_STUCK_BITS | IF);
    EXPECT_EQ(read_mem16(0x100 - 4, SS), 0x7c00);
    // Return to the following instruction
    EXPECT_EQ(read_mem16(0x100 - 6, SS), 0x0001 + GetParam().size());
}
INSTANTIATE_TEST_CASE_P(InvalidOpcode, InvalidOpcodeFixture,
    ::testing::Values(
        std::vector<uint8_t>{ 0x0f },
        std::vector<uint8_t>{ 0x63 },
        std::vector<uint8_t>{ 0x64 },
        std::vector<uint8_t>{ 0x65 },
        std::vector<uint8_t>{ 0x66 },
        std::vector<uint8_t>{ 0x67 },
        std::vector<uint8_t>{ 0xf1 },
        std::vector<uint8_t>{ 0xfe, 0xff },
        std::vector<uint8_t>{ 0xff, 0xff }
    ));

TEST_F(EmulateFixture, InvalidRegF6)
{
    set_instruction({ 0xf6, 1 << 3 });

    emulate();

    ASSERT_FALSE(instruction_had_side_effects());
}

TEST_F(EmulateFixture, InvalidRegF7)
{
    set_instruction({ 0xf7, 1 << 3 });

    emulate();

    ASSERT_FALSE(instruction_had_side_effects());
}

TEST_F(EmulateFixture, InvalidRegFE)
{
    set_instruction({ 0xfe, 2 << 3 });

    emulate();

    ASSERT_FALSE(instruction_had_side_effects());
}
