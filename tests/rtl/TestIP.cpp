#include <gtest/gtest.h>
#include <VIP.h>

#include "VerilogTestbench.h"

class IPTestFixture : public VerilogTestbench<VIP>,
    public ::testing::Test {
public:
    void inc();
    uint16_t get();
    void set(uint16_t v);
};

void IPTestFixture::inc()
{
    this->dut.inc = 1;
    after_n_cycles(1, [&]{
        this->dut.inc = 0;
    });
    cycle();
}

uint16_t IPTestFixture::get()
{
    return this->dut.val;
}

void IPTestFixture::set(uint16_t val)
{
    this->dut.wr_val = val;
    this->dut.wr_en = 1;
    after_n_cycles(1, [&]{
        this->dut.wr_en = 0;
    });
    cycle();
}

TEST_F(IPTestFixture, IncrementAndReset)
{
    ASSERT_EQ(0U, get());
    inc();
    inc();
    ASSERT_EQ(2U, get());

    reset();

    ASSERT_EQ(0U, get());
}

TEST_F(IPTestFixture, SetNewValue)
{
    ASSERT_EQ(0U, get());

    set(0xa55a);

    ASSERT_EQ(0xa55a, get());
}
