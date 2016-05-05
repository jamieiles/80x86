#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

#include "Fifo.h"
#include "MockRegisterFile.h"
#include "ModRM.h"

using ::testing::Return;

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

class ModRMTestFixture : public ::testing::Test {
public:
    ModRMTestFixture()
        : instr_stream(16)
    {
        decoder = std::make_unique<ModRMDecoder>(
            [&]{ return this->instr_stream.pop(); },
            &regs
        );
    }
protected:
    void validate_effective_address(uint16_t expected)
    {
        ASSERT_EQ(OP_MEM, decoder->rm_type());
        ASSERT_EQ(expected, decoder->effective_address());
        ASSERT_EQ(AX, decoder->reg());
    }
    Fifo<uint8_t> instr_stream;
    MockRegisterFile regs;
    std::unique_ptr<ModRMDecoder> decoder;
};

TEST_F(ModRMTestFixture, Regreg16)
{
    uint8_t modrm = (0x3 << 6) | (MODRM_REG_AX << 3) | (MODRM_REG_CX << 0);
    instr_stream.push(modrm);

    decoder->decode();
    ASSERT_EQ(OP_REG, decoder->rm_type());
    ASSERT_EQ(AX, decoder->reg());
    ASSERT_EQ(CX, decoder->rm_reg());
}

TEST_F(ModRMTestFixture, RegReg8)
{
    uint8_t modrm = (0x3 << 6) | (MODRM_REG_AL << 3) | (MODRM_REG_CH << 0);
    instr_stream.push(modrm);

    decoder->set_width(OP_WIDTH_8);
    decoder->decode();
    ASSERT_EQ(OP_REG, decoder->rm_type());
    ASSERT_EQ(AL, decoder->reg());
    ASSERT_EQ(CH, decoder->rm_reg());
}

////////////////////////////////////////////////////////////////////////////////
// mod=00
////////////////////////////////////////////////////////////////////////////////

TEST_F(ModRMTestFixture, Mod00RM000)
{
    EXPECT_CALL(regs, get(BX)).WillOnce(Return(0x100));
    EXPECT_CALL(regs, get(SI)).WillOnce(Return(0x4));

    instr_stream.push(0);
    decoder->decode();

    validate_effective_address(0x104);
}

TEST_F(ModRMTestFixture, Mod00RM001)
{
    EXPECT_CALL(regs, get(BX)).WillOnce(Return(0x100));
    EXPECT_CALL(regs, get(DI)).WillOnce(Return(0x4));

    instr_stream.push(0x01);
    decoder->decode();

    validate_effective_address(0x104);
}

TEST_F(ModRMTestFixture, Mod00RM010)
{
    EXPECT_CALL(regs, get(BP)).WillOnce(Return(0x100));
    EXPECT_CALL(regs, get(SI)).WillOnce(Return(0x4));

    instr_stream.push(0x02);
    decoder->decode();

    validate_effective_address(0x104);
}

TEST_F(ModRMTestFixture, Mod00RM011)
{
    EXPECT_CALL(regs, get(BP)).WillOnce(Return(0x100));
    EXPECT_CALL(regs, get(DI)).WillOnce(Return(0x4));

    instr_stream.push(0x03);
    decoder->decode();

    validate_effective_address(0x104);
}

TEST_F(ModRMTestFixture, Mod00RM100)
{
    EXPECT_CALL(regs, get(SI)).WillOnce(Return(0x100));

    instr_stream.push(0x04);
    decoder->decode();

    validate_effective_address(0x100);
}

TEST_F(ModRMTestFixture, Mod00RM101)
{
    EXPECT_CALL(regs, get(DI)).WillOnce(Return(0x100));

    instr_stream.push(0x05);
    decoder->decode();

    validate_effective_address(0x100);
}

TEST_F(ModRMTestFixture, Mod00RM110)
{
    instr_stream.push(0x06);
    instr_stream.push(0x34);
    instr_stream.push(0x12);

    decoder->decode();

    validate_effective_address(0x1234);
}

TEST_F(ModRMTestFixture, Mod00RM111)
{
    EXPECT_CALL(regs, get(BX)).WillOnce(Return(0x100));

    instr_stream.push(0x07);
    decoder->decode();

    validate_effective_address(0x100);
}

////////////////////////////////////////////////////////////////////////////////
// mod=01
////////////////////////////////////////////////////////////////////////////////

TEST_F(ModRMTestFixture, Mod01RM000)
{
    EXPECT_CALL(regs, get(BX)).WillOnce(Return(0x100));
    EXPECT_CALL(regs, get(SI)).WillOnce(Return(0x4));

    instr_stream.push(0x40);
    instr_stream.push(0xff);
    decoder->decode();

    validate_effective_address(0x103);
}

TEST_F(ModRMTestFixture, Mod01RM001)
{
    EXPECT_CALL(regs, get(BX)).WillOnce(Return(0x100));
    EXPECT_CALL(regs, get(DI)).WillOnce(Return(0x4));

    instr_stream.push(0x41);
    instr_stream.push(0xff);
    decoder->decode();

    validate_effective_address(0x103);
}

TEST_F(ModRMTestFixture, Mod01RM010)
{
    EXPECT_CALL(regs, get(BP)).WillOnce(Return(0x100));
    EXPECT_CALL(regs, get(SI)).WillOnce(Return(0x4));

    instr_stream.push(0x42);
    instr_stream.push(0xff);
    decoder->decode();

    validate_effective_address(0x103);
}

TEST_F(ModRMTestFixture, Mod01RM011)
{
    EXPECT_CALL(regs, get(BP)).WillOnce(Return(0x100));
    EXPECT_CALL(regs, get(DI)).WillOnce(Return(0x4));

    instr_stream.push(0x43);
    instr_stream.push(0xff);
    decoder->decode();

    validate_effective_address(0x103);
}

TEST_F(ModRMTestFixture, Mod01RM100)
{
    EXPECT_CALL(regs, get(SI)).WillOnce(Return(0x100));

    instr_stream.push(0x44);
    instr_stream.push(0xff);
    decoder->decode();

    validate_effective_address(0xff);
}

TEST_F(ModRMTestFixture, Mod01RM101)
{
    EXPECT_CALL(regs, get(DI)).WillOnce(Return(0x100));

    instr_stream.push(0x45);
    instr_stream.push(0xff);
    decoder->decode();

    validate_effective_address(0xff);
}

TEST_F(ModRMTestFixture, Mod01RM110)
{
    EXPECT_CALL(regs, get(BP)).WillOnce(Return(0x100));

    instr_stream.push(0x46);
    instr_stream.push(0xff);

    decoder->decode();

    validate_effective_address(0xff);
}

TEST_F(ModRMTestFixture, Mod01RM111)
{
    EXPECT_CALL(regs, get(BX)).WillOnce(Return(0x100));

    instr_stream.push(0x47);
    instr_stream.push(0xff);
    decoder->decode();

    validate_effective_address(0xff);
}

////////////////////////////////////////////////////////////////////////////////
// mod=10
////////////////////////////////////////////////////////////////////////////////

TEST_F(ModRMTestFixture, Mod10RM000)
{
    EXPECT_CALL(regs, get(BX)).WillOnce(Return(0x100));
    EXPECT_CALL(regs, get(SI)).WillOnce(Return(0x4));

    instr_stream.push(0x80);
    instr_stream.push(0xff);
    instr_stream.push(0xff);
    decoder->decode();

    validate_effective_address(0x103);
}

TEST_F(ModRMTestFixture, Mod10RM001)
{
    EXPECT_CALL(regs, get(BX)).WillOnce(Return(0x100));
    EXPECT_CALL(regs, get(DI)).WillOnce(Return(0x4));

    instr_stream.push(0x81);
    instr_stream.push(0xff);
    instr_stream.push(0xff);
    decoder->decode();

    validate_effective_address(0x103);
}

TEST_F(ModRMTestFixture, Mod10RM010)
{
    EXPECT_CALL(regs, get(BP)).WillOnce(Return(0x100));
    EXPECT_CALL(regs, get(SI)).WillOnce(Return(0x4));

    instr_stream.push(0x82);
    instr_stream.push(0xff);
    instr_stream.push(0xff);
    decoder->decode();

    validate_effective_address(0x103);
}

TEST_F(ModRMTestFixture, Mod10RM011)
{
    EXPECT_CALL(regs, get(BP)).WillOnce(Return(0x100));
    EXPECT_CALL(regs, get(DI)).WillOnce(Return(0x4));

    instr_stream.push(0x83);
    instr_stream.push(0xff);
    instr_stream.push(0xff);
    decoder->decode();

    validate_effective_address(0x103);
}

TEST_F(ModRMTestFixture, Mod10RM100)
{
    EXPECT_CALL(regs, get(SI)).WillOnce(Return(0x100));

    instr_stream.push(0x84);
    instr_stream.push(0xff);
    instr_stream.push(0xff);
    decoder->decode();

    validate_effective_address(0xff);
}

TEST_F(ModRMTestFixture, Mod10RM101)
{
    EXPECT_CALL(regs, get(DI)).WillOnce(Return(0x100));

    instr_stream.push(0x85);
    instr_stream.push(0xff);
    instr_stream.push(0xff);
    decoder->decode();

    validate_effective_address(0xff);
}

TEST_F(ModRMTestFixture, Mod10RM110)
{
    EXPECT_CALL(regs, get(BP)).WillOnce(Return(0x100));

    instr_stream.push(0x86);
    instr_stream.push(0xff);
    instr_stream.push(0xff);

    decoder->decode();

    validate_effective_address(0xff);
}

TEST_F(ModRMTestFixture, Mod10RM111)
{
    EXPECT_CALL(regs, get(BX)).WillOnce(Return(0x100));

    instr_stream.push(0x87);
    instr_stream.push(0xff);
    instr_stream.push(0xff);
    decoder->decode();

    validate_effective_address(0xff);
}
