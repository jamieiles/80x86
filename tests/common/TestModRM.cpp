#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestModRM.h"

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

template <typename T>
class ModRMTestFixture : public ::testing::Test {
public:
    void validate_effective_address(const T &decoder,
                                    uint16_t expected)
    {
        ASSERT_EQ(OP_MEM, decoder.get_rm_type());
        ASSERT_EQ(expected, decoder.get_effective_address());
        ASSERT_EQ(AX, decoder.get_register());
    }
};
TYPED_TEST_CASE_P(ModRMTestFixture);

TYPED_TEST_P(ModRMTestFixture, Regreg16)
{
    TypeParam decoder;

    uint8_t modrm = (0x3 << 6) | (MODRM_REG_AX << 3) | (MODRM_REG_CX << 0);
    decoder.set_instruction({ modrm });

    decoder.decode();
    ASSERT_EQ(OP_REG, decoder.get_rm_type());
    ASSERT_EQ(AX, decoder.get_register());
    ASSERT_EQ(CX, decoder.get_rm_register());
}

TYPED_TEST_P(ModRMTestFixture, RegReg8)
{
    TypeParam decoder;

    uint8_t modrm = (0x3 << 6) | (MODRM_REG_AL << 3) | (MODRM_REG_CH << 0);
    decoder.set_instruction({ modrm });

    decoder.set_width(OP_WIDTH_8);
    decoder.decode();
    ASSERT_EQ(OP_REG, decoder.get_rm_type());
    ASSERT_EQ(AL, decoder.get_register());
    ASSERT_EQ(CH, decoder.get_rm_register());
}

////////////////////////////////////////////////////////////////////////////////
// mod=00
////////////////////////////////////////////////////////////////////////////////

TYPED_TEST_P(ModRMTestFixture, Mod00RM000)
{
    TypeParam decoder;

    decoder.get_registers()->set(BX, 0x100);
    decoder.get_registers()->set(SI, 0x4);

    decoder.set_instruction({ 0 });
    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0x104);
}

TYPED_TEST_P(ModRMTestFixture, Mod00RM001)
{
    TypeParam decoder;

    decoder.get_registers()->set(BX, 0x100);
    decoder.get_registers()->set(DI, 0x4);

    decoder.set_instruction({ 0x01 });
    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0x104);
}

TYPED_TEST_P(ModRMTestFixture, Mod00RM010)
{
    TypeParam decoder;

    decoder.get_registers()->set(BP, 0x100);
    decoder.get_registers()->set(SI, 0x004);

    decoder.set_instruction({ 0x02 });
    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0x104);
}

TYPED_TEST_P(ModRMTestFixture, Mod00RM011)
{
    TypeParam decoder;

    decoder.get_registers()->set(BP, 0x100);
    decoder.get_registers()->set(DI, 0x4);

    decoder.set_instruction({ 0x03 });
    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0x104);
}

TYPED_TEST_P(ModRMTestFixture, Mod00RM100)
{
    TypeParam decoder;

    decoder.get_registers()->set(SI, 0x100);

    decoder.set_instruction({ 0x04 });
    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0x100);
}

TYPED_TEST_P(ModRMTestFixture, Mod00RM101)
{
    TypeParam decoder;

    decoder.get_registers()->set(DI, 0x100);

    decoder.set_instruction({ 0x05 });
    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0x100);
}

TYPED_TEST_P(ModRMTestFixture, Mod00RM110)
{
    TypeParam decoder;

    decoder.set_instruction({ 0x06, 0x34, 0x12 });

    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0x1234);
}

TYPED_TEST_P(ModRMTestFixture, Mod00RM111)
{
    TypeParam decoder;

    decoder.get_registers()->set(BX, 0x100);

    decoder.set_instruction({ 0x07 });
    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0x100);
}

////////////////////////////////////////////////////////////////////////////////
// mod=01
////////////////////////////////////////////////////////////////////////////////

TYPED_TEST_P(ModRMTestFixture, Mod01RM000)
{
    TypeParam decoder;

    decoder.get_registers()->set(BX, 0x100);
    decoder.get_registers()->set(SI, 0x4);

    decoder.set_instruction({ 0x40, 0xff });
    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0x103);
}

TYPED_TEST_P(ModRMTestFixture, Mod01RM001)
{
    TypeParam decoder;

    decoder.get_registers()->set(BX, 0x100);
    decoder.get_registers()->set(DI, 0x4);

    decoder.set_instruction({ 0x41, 0xff });
    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0x103);
}

TYPED_TEST_P(ModRMTestFixture, Mod01RM010)
{
    TypeParam decoder;

    decoder.get_registers()->set(BP, 0x100);
    decoder.get_registers()->set(SI, 0x4);

    decoder.set_instruction({ 0x42, 0xff });
    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0x103);
}

TYPED_TEST_P(ModRMTestFixture, Mod01RM011)
{
    TypeParam decoder;

    decoder.get_registers()->set(BP, 0x100);
    decoder.get_registers()->set(DI, 0x4);

    decoder.set_instruction({ 0x43, 0xff });
    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0x103);
}

TYPED_TEST_P(ModRMTestFixture, Mod01RM100)
{
    TypeParam decoder;

    decoder.get_registers()->set(SI, 0x100);

    decoder.set_instruction({ 0x44, 0xff });
    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0xff);
}

TYPED_TEST_P(ModRMTestFixture, Mod01RM101)
{
    TypeParam decoder;

    decoder.get_registers()->set(DI, 0x100);

    decoder.set_instruction({ 0x45, 0xff });
    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0xff);
}

TYPED_TEST_P(ModRMTestFixture, Mod01RM110)
{
    TypeParam decoder;

    decoder.get_registers()->set(BP, 0x100);

    decoder.set_instruction({ 0x46, 0xff });

    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0xff);
}

TYPED_TEST_P(ModRMTestFixture, Mod01RM111)
{
    TypeParam decoder;

    decoder.get_registers()->set(BX, 0x100);

    decoder.set_instruction({ 0x47, 0xff });
    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0xff);
}

////////////////////////////////////////////////////////////////////////////////
// mod=10
////////////////////////////////////////////////////////////////////////////////

TYPED_TEST_P(ModRMTestFixture, Mod10RM000)
{
    TypeParam decoder;

    decoder.get_registers()->set(BX, 0x100);
    decoder.get_registers()->set(SI, 0x4);

    decoder.set_instruction({ 0x80, 0xff, 0xff });
    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0x103);
}

TYPED_TEST_P(ModRMTestFixture, Mod10RM001)
{
    TypeParam decoder;

    decoder.get_registers()->set(BX, 0x100);
    decoder.get_registers()->set(DI, 0x4);

    decoder.set_instruction({ 0x81, 0xff, 0xff });
    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0x103);
}

TYPED_TEST_P(ModRMTestFixture, Mod10RM010)
{
    TypeParam decoder;

    decoder.get_registers()->set(BP, 0x100);
    decoder.get_registers()->set(SI, 0x4);

    decoder.set_instruction({ 0x82, 0xff, 0xff });
    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0x103);
}

TYPED_TEST_P(ModRMTestFixture, Mod10RM011)
{
    TypeParam decoder;

    decoder.get_registers()->set(BP, 0x100);
    decoder.get_registers()->set(DI, 0x4);

    decoder.set_instruction({ 0x83, 0xff, 0xff });
    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0x103);
}

TYPED_TEST_P(ModRMTestFixture, Mod10RM100)
{
    TypeParam decoder;

    decoder.get_registers()->set(SI, 0x100);

    decoder.set_instruction({ 0x84, 0xff, 0xff });
    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0xff);
}

TYPED_TEST_P(ModRMTestFixture, Mod10RM101)
{
    TypeParam decoder;

    decoder.get_registers()->set(DI, 0x100);

    decoder.set_instruction({ 0x85, 0xff, 0xff });
    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0xff);
}

TYPED_TEST_P(ModRMTestFixture, Mod10RM110)
{
    TypeParam decoder;

    decoder.get_registers()->set(BP, 0x100);

    decoder.set_instruction({ 0x86, 0xff, 0xff });

    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0xff);
}

TYPED_TEST_P(ModRMTestFixture, Mod10RM111)
{
    TypeParam decoder;

    decoder.get_registers()->set(BX, 0x100);

    decoder.set_instruction({ 0x87, 0x78, 0x20 });
    decoder.decode();

    TestFixture::validate_effective_address(decoder, 0x2178);
}

REGISTER_TYPED_TEST_CASE_P(ModRMTestFixture,
                           Regreg16,
                           RegReg8,
                           Mod00RM000,
                           Mod00RM001,
                           Mod00RM010,
                           Mod00RM011,
                           Mod00RM100,
                           Mod00RM101,
                           Mod00RM110,
                           Mod00RM111,
                           Mod01RM000,
                           Mod01RM001,
                           Mod01RM010,
                           Mod01RM011,
                           Mod01RM100,
                           Mod01RM101,
                           Mod01RM110,
                           Mod01RM111,
                           Mod10RM000,
                           Mod10RM001,
                           Mod10RM010,
                           Mod10RM011,
                           Mod10RM100,
                           Mod10RM101,
                           Mod10RM110,
                           Mod10RM111);
