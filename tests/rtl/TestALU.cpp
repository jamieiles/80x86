#include <gtest/gtest.h>
#include <VALU.h>

#include "VerilogTestbench.h"
#include "MicrocodeTypes.h"

class ALUTestFixture : public ::testing::Test {
protected:
    VALU dut;
};

TEST_F(ALUTestFixture, Select)
{
    dut.a = 0xaaaa;
    dut.b = 0xbbbb;

    dut.op = ALUOp_SELA;
    dut.eval();
    ASSERT_EQ(dut.out, 0xaaaa);

    dut.op = ALUOp_SELB;
    dut.eval();
    ASSERT_EQ(dut.out, 0xbbbb);
}
