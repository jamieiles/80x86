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

TEST(Fifo, empty_fifo_not_full)
{
    VFifo dut;
    FifoTestbench tb(&dut);

    ASSERT_TRUE(tb.dut->empty);
    ASSERT_FALSE(tb.dut->full);
}

TEST(Fifo, one_push_not_empty)
{
    VFifo dut;
    FifoTestbench tb(&dut);

    ASSERT_TRUE(tb.dut->empty);
    tb.push(0xdeadbeef);
    ASSERT_FALSE(tb.dut->empty);
}

TEST(Fifo, one_push_one_pop)
{
    VFifo dut;
    FifoTestbench tb(&dut);

    ASSERT_TRUE(tb.dut->empty);
    ASSERT_EQ(tb.dut->rd_data, 0LU);

    tb.push(0xdeadbeef);
    ASSERT_FALSE(tb.dut->empty);

    auto v = tb.pop();
    ASSERT_TRUE(tb.dut->empty);
    ASSERT_EQ(v, 0xdeadbeef);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
