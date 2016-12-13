#include <gtest/gtest.h>
#include <VFifo.h>

#include "VerilogTestbench.h"

class FifoTestbench : public VerilogTestbench<VFifo> {
public:
    FifoTestbench(VFifo *dut);
    void push(uint32_t val);
    uint32_t pop();
};

FifoTestbench::FifoTestbench(VFifo *dut)
    : VerilogTestbench<VFifo>(dut)
{
    dut->wr_en = 0;
    dut->wr_data = 0LU;
    dut->rd_en = 0;
}

void FifoTestbench::push(uint32_t val)
{
    dut->wr_data = val;
    dut->wr_en = 1;
    cycle();
    dut->wr_en = 0;
}

uint32_t FifoTestbench::pop()
{
    dut->rd_en = 1;
    cycle();
    dut->rd_en = 0;

    return dut->rd_data;
}

class FifoTestFixture : public ::testing::Test {
public:
    FifoTestFixture()
        : tb(&fifo) {}
protected:
    VFifo fifo;
    FifoTestbench tb;
};

TEST_F(FifoTestFixture, empty_fifo_not_full)
{
    ASSERT_TRUE(tb.dut->empty);
    ASSERT_FALSE(tb.dut->full);
}

TEST_F(FifoTestFixture, one_push_not_empty)
{
    ASSERT_TRUE(tb.dut->empty);
    tb.push(0xdeadbeef);
    ASSERT_FALSE(tb.dut->empty);
}

TEST_F(FifoTestFixture, one_push_one_pop)
{
    ASSERT_TRUE(tb.dut->empty);
    ASSERT_EQ(tb.dut->rd_data, 0LU);

    tb.push(0xdeadbeef);
    ASSERT_FALSE(tb.dut->empty);

    auto v = tb.pop();
    ASSERT_TRUE(tb.dut->empty);
    ASSERT_EQ(v, 0xdeadbeef);
}

TEST_F(FifoTestFixture, push3pop3)
{
    for (uint32_t m = 0; m < 3; ++m)
        tb.push(m << 16);

    ASSERT_FALSE(tb.dut->empty);
    ASSERT_FALSE(tb.dut->full);

    for (uint32_t m = 0; m < 3; ++m)
        ASSERT_EQ(tb.pop(), m << 16);

    ASSERT_TRUE(tb.dut->empty);
}

TEST_F(FifoTestFixture, underflow_still_empty)
{
    ASSERT_TRUE(tb.dut->empty);
    tb.pop();
    ASSERT_TRUE(tb.dut->empty);
}

TEST_F(FifoTestFixture, overflow_no_corrupt)
{
    for (uint32_t m = 0; m < 10; ++m)
        tb.push(m);

    ASSERT_TRUE(tb.dut->full);
    ASSERT_FALSE(tb.dut->empty);

    for (uint32_t m = 0; m < 8; ++m)
        ASSERT_EQ(tb.pop(), m);
    ASSERT_TRUE(tb.dut->empty);
    ASSERT_FALSE(tb.dut->full);
}

TEST_F(FifoTestFixture, read_during_write)
{
    tb.push(0x1234);

    tb.dut->wr_en = 1;
    tb.dut->wr_data = 0xaa55;
    tb.dut->rd_en = 1;
    tb.cycle();
    tb.dut->wr_en = 0;

    ASSERT_EQ(tb.dut->rd_data, 0x1234LU);
    tb.cycle();
    ASSERT_EQ(tb.pop(), 0xaa55LU);
}

TEST_F(FifoTestFixture, fill_at_threshold)
{
    for (uint32_t m = 0; m < 6; ++m)
        tb.push(m);

    ASSERT_TRUE(tb.dut->nearly_full);
}
