#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

#include "Fifo.h"
#include "RegisterFile.h"
#include "ModRM.h"

using ::testing::Return;

class MockRegisterFile : public RegisterFile {
public:
    MockRegisterFile()
        : RegisterFile()
    {}
    MOCK_METHOD0(reset, void());
    MOCK_METHOD2(set, void(GPR, uint16_t));
    MOCK_CONST_METHOD1(get, uint16_t(GPR));
    MOCK_CONST_METHOD0(get_flags, uint16_t());
    MOCK_METHOD1(set_flags, void(uint16_t));
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
