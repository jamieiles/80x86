#include <gtest/gtest.h>
#include <VFifo.h>

#include "VerilogTestbench.h"

class FifoTestFixture : public VerilogTestbench<VFifo>,
    public ::testing::Test {
public:
    FifoTestFixture();
    void push(uint32_t val);
    bool is_empty() const
    {
        return empty;
    }
    uint32_t get_head() const
    {
        return head;
    }
    uint32_t pop();
private:
    bool empty;
    uint32_t head;
};

FifoTestFixture::FifoTestFixture()
    : empty(true),
    head(0x7777)
{
    dut.wr_en = 0;
    dut.wr_data = 0LU;
    dut.rd_en = 0;

    periodic(ClockCapture, [&]{
        this->empty = this->dut.empty;
        this->head = this->dut.rd_data;
    });

    reset();
}

void FifoTestFixture::push(uint32_t val)
{
    after_n_cycles(0, [&]{
        this->dut.wr_data = val;
        this->dut.wr_en = 1;
        after_n_cycles(1, [&]{
            this->dut.wr_en = 0;
        });
    });
    cycle(2);
}

uint32_t FifoTestFixture::pop()
{
    while (is_empty())
        cycle();
    auto v = head;

    after_n_cycles(0, [&]{
        this->dut.rd_en = 1;
        after_n_cycles(1, [&]{
            this->dut.rd_en = 0;
        });
    });
    cycle(2);

    return v;
}

TEST_F(FifoTestFixture, empty_fifo_not_full)
{
    ASSERT_TRUE(dut.empty);
    ASSERT_FALSE(dut.full);
}

TEST_F(FifoTestFixture, one_push_not_empty)
{
    ASSERT_TRUE(is_empty());
    push(0xdeadbeef);
    cycle();
    ASSERT_FALSE(is_empty());
}

TEST_F(FifoTestFixture, one_push_one_pop)
{
    ASSERT_TRUE(dut.empty);
    ASSERT_EQ(dut.rd_data, 0LU);

    push(0xdeadbeef);
    cycle();
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

TEST_F(FifoTestFixture, overflow_no_corrupt)
{
    for (uint32_t m = 0; m < 8; ++m)
        push(m);

    ASSERT_TRUE(dut.full);
    ASSERT_FALSE(dut.empty);

    for (uint32_t m = 0; m < 6; ++m)
        ASSERT_EQ(pop(), m);
    cycle();
    ASSERT_TRUE(is_empty());
    ASSERT_FALSE(dut.full);
}

TEST_F(FifoTestFixture, read_during_write)
{
    push(0x1234);

    after_n_cycles(0, [&]{
        this->dut.wr_en = 1;
        this->dut.wr_data = 0xaa55;
        this->dut.rd_en = 1;
        after_n_cycles(1, [&]{
            this->dut.wr_en = 0;
            this->dut.rd_en = 0;
        });
    });
    ASSERT_EQ(pop(), 0x1234LU);
    ASSERT_EQ(pop(), 0xaa55LU);
}

TEST_F(FifoTestFixture, fill_at_threshold)
{
    int pushed = 0;

    for (;;) {
        push(1);
        ++pushed;
        if (dut.nearly_full)
            break;
    }

    ASSERT_EQ(pushed, 4);
}

TEST_F(FifoTestFixture, reset_empties)
{
    for (uint32_t m = 0; m < 4; ++m)
        push(m);

    ASSERT_FALSE(dut.empty);
    reset();
    ASSERT_TRUE(dut.empty);
}
