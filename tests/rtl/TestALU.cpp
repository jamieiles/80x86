#include <gtest/gtest.h>
#include <VALU.h>

#include "VerilogTestbench.h"

class ALUTestFixture : public ::testing::Test {
protected:
    VALU dut;
};

TEST_F(ALUTestFixture, Select)
{
    dut.a = 0xaaaa;
    dut.b = 0xbbbb;

    dut.op = 2;
    dut.eval();
    ASSERT_EQ(dut.out, 0xaaaa);

    dut.op = 3;
    dut.eval();
    ASSERT_EQ(dut.out, 0xbbbb);
}
