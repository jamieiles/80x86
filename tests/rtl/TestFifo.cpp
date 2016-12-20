#include <gtest/gtest.h>
#include <VFifo.h>

#include "VerilogTestbench.h"

class FifoTestFixture : public VerilogTestbench<VFifo>,
    public ::testing::Test {
public:
    FifoTestFixture();
    void push(uint32_t val);
    uint32_t pop();
};

FifoTestFixture::FifoTestFixture()
{
    dut.wr_en = 0;
    dut.wr_data = 0LU;
    dut.rd_en = 0;
}

void FifoTestFixture::push(uint32_t val)
{
    dut.wr_data = val;
    dut.wr_en = 1;
    cycle();
    dut.wr_en = 0;
}

uint32_t FifoTestFixture::pop()
{
    dut.rd_en = 1;
    cycle();
    dut.rd_en = 0;

    return dut.rd_data;
}

TEST_F(FifoTestFixture, empty_fifo_not_full)
{
    ASSERT_TRUE(dut.empty);
    ASSERT_FALSE(dut.full);
}

TEST_F(FifoTestFixture, one_push_not_empty)
{
    ASSERT_TRUE(dut.empty);
    push(0xdeadbeef);
    ASSERT_FALSE(dut.empty);
}

TEST_F(FifoTestFixture, one_push_one_pop)
{
    ASSERT_TRUE(dut.empty);
    ASSERT_EQ(dut.rd_data, 0LU);

    push(0xdeadbeef);
    ASSERT_FALSE(dut.empty);

    auto v = pop();
    ASSERT_TRUE(dut.empty);
    ASSERT_EQ(v, 0xdeadbeef);
}

TEST_F(FifoTestFixture, push3pop3)
{
    for (uint32_t m = 0; m < 3; ++m)
        push(m << 16);

    ASSERT_FALSE(dut.empty);
    ASSERT_FALSE(dut.full);

    for (uint32_t m = 0; m < 3; ++m)
        ASSERT_EQ(pop(), m << 16);

    ASSERT_TRUE(dut.empty);
}

TEST_F(FifoTestFixture, underflow_still_empty)
{
    ASSERT_TRUE(dut.empty);
    pop();
    ASSERT_TRUE(dut.empty);
}

TEST_F(FifoTestFixture, overflow_no_corrupt)
{
    for (uint32_t m = 0; m < 10; ++m)
        push(m);

    ASSERT_TRUE(dut.full);
    ASSERT_FALSE(dut.empty);

    for (uint32_t m = 0; m < 8; ++m)
        ASSERT_EQ(pop(), m);
    ASSERT_TRUE(dut.empty);
    ASSERT_FALSE(dut.full);
}

TEST_F(FifoTestFixture, read_during_write)
{
    push(0x1234);

    dut.wr_en = 1;
    dut.wr_data = 0xaa55;
    dut.rd_en = 1;
    cycle();
    dut.wr_en = 0;

    ASSERT_EQ(dut.rd_data, 0x1234LU);
    cycle();
    ASSERT_EQ(pop(), 0xaa55LU);
}

TEST_F(FifoTestFixture, fill_at_threshold)
{
    int pushed = 0;

    for (;;) {
        push(1);
        if (dut.nearly_full)
            break;
        ++pushed;
    }

    ASSERT_EQ(pushed, 6);
}

TEST_F(FifoTestFixture, reset_empties)
{
    for (uint32_t m = 0; m < 4; ++m)
        push(m);

    ASSERT_FALSE(dut.empty);
    reset();
    ASSERT_TRUE(dut.empty);
}
