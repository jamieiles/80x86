#include <gtest/gtest.h>
#include <VCSIPSync.h>

#include "VerilogTestbench.h"

class CSIPSyncTestFixture : public VerilogTestbench<VCSIPSync>,
    public ::testing::Test {
public:
    CSIPSyncTestFixture();
protected:
    void update_ip(uint16_t newip);
    void update_cs();
    void propagate();
    bool updated;
    uint16_t updated_ip;
};

CSIPSyncTestFixture::CSIPSyncTestFixture()
    : updated(false),
    updated_ip(true)
{
    reset();

    periodic(ClockCapture, [&]{
        if (this->dut.update_out) {
            this->updated = true;
            this->updated_ip = this->dut.ip_out;
        }
    });
}

void CSIPSyncTestFixture::update_ip(uint16_t newip)
{
    after_n_cycles(0, [&]{
        this->dut.new_ip = newip;
        this->dut.ip_update = 1;
        after_n_cycles(1, [&]{
            this->dut.ip_update = 0;
            this->dut.new_ip = 0;
        });
    });
    cycle(10);
}

void CSIPSyncTestFixture::update_cs()
{
    after_n_cycles(0, [&]{
        this->dut.cs_update = 1;
        after_n_cycles(1, [&]{
            this->dut.cs_update = 0;
        });
    });
    cycle(10);
}

void CSIPSyncTestFixture::propagate()
{
    after_n_cycles(0, [&]{
        this->dut.propagate = 1;
        after_n_cycles(1, [&]{
            this->dut.propagate = 0;
        });
    });
    cycle();
}

TEST_F(CSIPSyncTestFixture, NoChangeNoUpdate)
{
    propagate();

    ASSERT_FALSE(updated);
}

TEST_F(CSIPSyncTestFixture, IPPropagateDelayed)
{
    update_ip(0x1234);
    propagate();

    EXPECT_TRUE(updated);
    EXPECT_EQ(updated_ip, 0x1234);

    cycle();
}

TEST_F(CSIPSyncTestFixture, CSPropagateDelayed)
{
    update_cs();
    propagate();

    EXPECT_TRUE(updated);
    EXPECT_EQ(updated_ip, 0);

    cycle();
}
