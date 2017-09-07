#include <gtest/gtest.h>
#include <VPosedgeToPulse.h>

#include "VerilogTestbench.h"

class PosedgeToPulseTestFixture : public VerilogTestbench<VPosedgeToPulse>,
                                  public ::testing::Test
{
public:
    PosedgeToPulseTestFixture();
};

PosedgeToPulseTestFixture::PosedgeToPulseTestFixture()
{
    reset();
}

TEST_F(PosedgeToPulseTestFixture, IdleNoPulse)
{
    for (auto i = 0; i < 32; ++i) {
        cycle();
        EXPECT_EQ(dut.q, 0);
    }
}

TEST_F(PosedgeToPulseTestFixture, PosedgeTriggersOnce)
{
    EXPECT_EQ(dut.q, 0);

    after_n_cycles(0, [&] { this->dut.d = 1; });
    cycle(2);
    EXPECT_EQ(this->dut.q, 1);

    for (auto i = 0; i < 32; ++i) {
        cycle();
        EXPECT_EQ(this->dut.q, 0);
    }
}

TEST_F(PosedgeToPulseTestFixture, PosedgePulseTriggersN)
{
    EXPECT_EQ(dut.q, 0);

    for (auto i = 0; i < 8; ++i) {
        after_n_cycles(0, [&] {
            this->dut.d = 1;
            after_n_cycles(1, [&] { this->dut.d = 0; });
        });
        cycle(2);
        EXPECT_EQ(this->dut.q, 1);

        for (auto j = 0; j < 32; ++j) {
            cycle();
            EXPECT_EQ(this->dut.q, 0);
        }
    }
}
