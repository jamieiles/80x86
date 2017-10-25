#include <stdexcept>
#include <deque>
#include <VPIC.h>
#include "VerilogTestbench.h"

class PICTestBench : public VerilogTestbench<VPIC>, public ::testing::Test
{
public:
    PICTestBench()
    {
        reset();
    }

protected:
    uint8_t access(int offs, bool write = false, uint8_t wr_val = 0)
    {
        uint16_t v;

        after_n_cycles(0, [this, &v, offs, wr_val, write] {
            this->dut.cs = 1;
            this->dut.data_m_data_in = static_cast<uint16_t>(wr_val)
                                       << (offs * 8);
            this->dut.data_m_bytesel = 1 << offs;
            this->dut.data_m_wr_en = write;
            this->dut.data_m_access = 1;

            after_n_cycles(1, [this, &v] {
                this->dut.cs = 0;
                this->dut.data_m_access = 0;
                v = this->dut.data_m_data_out;
            });
        });

        this->cycle(2);

        return v >> (offs * 8);
    }

    void write_reg(int offs, uint8_t v)
    {
        access(offs, true, v);
    }

    uint8_t read_reg(int offs)
    {
        return access(offs);
    }

    uint8_t read_irr()
    {
        write_reg(0, 0x0a);

        return read_reg(0);
    }

    uint8_t read_isr()
    {
        write_reg(0, 0x0b);

        return read_reg(0);
    }

    void set_mask(uint8_t v)
    {
        write_reg(1, v);
    }

    void setup_watch_and_ack()
    {
        periodic(ClockSetup, [this] {
            if (this->dut.intr) {
                after_n_cycles(0, [this] {
                    this->dut.inta = 1;
                    irqs_taken.push_back(this->dut.irq);
                    after_n_cycles(1, [this] {
                        this->dut.inta = 0;
                        this->dut.intr_in &=
                            ~(1 << ((*std::prev(irqs_taken.end())) - 8));
                    });
                });
            }
        });
    }

    std::deque<int> irqs_taken;
};

TEST_F(PICTestBench, Init)
{
    write_reg(0, 0x13);
    write_reg(1, 0x08);
    write_reg(1, 0x01);

    this->cycle(8);

    ASSERT_FALSE(this->dut.intr);
    ASSERT_EQ(this->dut.irq, 0);
}

TEST_F(PICTestBench, IRQVector)
{
    write_reg(0, 0x13);
    write_reg(1, 0x08);
    write_reg(1, 0x01);

    set_mask(0xff);

    this->cycle(2);
    this->dut.intr_in = (1 << 3);
    this->cycle(2);

    ASSERT_TRUE(this->dut.intr);
    ASSERT_EQ(this->dut.irq, 8 + 3);
}

TEST_F(PICTestBench, IRQPriority)
{
    write_reg(0, 0x13);
    write_reg(1, 0x08);
    write_reg(1, 0x01);

    set_mask(0xff);

    this->cycle(2);
    this->dut.intr_in = (1 << 3);
    this->cycle(2);
    this->dut.intr_in = (1 << 0);
    this->cycle(2);
    // Two unacknowledged interrupts

    ASSERT_TRUE(this->dut.intr);
    ASSERT_EQ(this->dut.irq, 8 + 0);
}

TEST_F(PICTestBench, ReadIRR)
{
    write_reg(0, 0x13);
    write_reg(1, 0x08);
    write_reg(1, 0x01);

    set_mask(0xff);

    this->cycle(2);
    this->dut.intr_in = (1 << 3);
    this->cycle(2);
    this->dut.intr_in = (1 << 0);
    this->cycle(2);
    // Two unacknowledged interrupts

    ASSERT_EQ(read_irr(), (1 << 3) | (1 << 0));
    ASSERT_EQ(read_isr(), 0);
}

TEST_F(PICTestBench, MaskedIntIgnored)
{
    write_reg(0, 0x13);
    write_reg(1, 0x08);
    write_reg(1, 0x01);
    set_mask(0);

    this->cycle(2);
    this->dut.intr_in = (1 << 3);
    this->cycle(2);

    ASSERT_FALSE(this->dut.intr);
}

TEST_F(PICTestBench, NoEOISuppressesSecondIRQ)
{
    write_reg(0, 0x13);
    write_reg(1, 0x08);
    write_reg(1, 0x01);
    set_mask(0xff);

    this->cycle(2);
    this->dut.intr_in = (1 << 3);
    this->cycle(1);

    setup_watch_and_ack();
    while (!irqs_taken.size())
        cycle();

    ASSERT_FALSE(this->dut.intr);
    ASSERT_EQ(irqs_taken[0], 8 + 3);
}

TEST_F(PICTestBench, NoEOIInhibitsLowerPriority)
{
    write_reg(0, 0x13);
    write_reg(1, 0x08);
    write_reg(1, 0x01);
    set_mask(0xff);

    this->cycle(2);
    this->dut.intr_in = (1 << 3);
    this->cycle(1);

    setup_watch_and_ack();
    while (!irqs_taken.size())
        cycle();

    this->dut.intr_in |= (1 << 7);
    cycle(32);

    ASSERT_FALSE(this->dut.intr);
    ASSERT_EQ(irqs_taken.size(), 1LU);
    ASSERT_EQ(irqs_taken[0], 8 + 3);
}

TEST_F(PICTestBench, NoEOIHigherPriorityTaken)
{
    write_reg(0, 0x13);
    write_reg(1, 0x08);
    write_reg(1, 0x01);
    set_mask(0xff);

    this->cycle(2);
    this->dut.intr_in = (1 << 3);
    this->cycle(1);

    setup_watch_and_ack();
    while (!irqs_taken.size())
        cycle();

    after_n_cycles(0, [this] { this->dut.intr_in |= (1 << 0); });
    cycle(32);

    ASSERT_FALSE(this->dut.intr);
    ASSERT_EQ(irqs_taken.size(), 2LU);
    ASSERT_EQ(irqs_taken[0], 8 + 3);
    ASSERT_EQ(irqs_taken[1], 8 + 0);
    ASSERT_EQ(this->dut.intr_in, 0);
}

TEST_F(PICTestBench, EOIReRaises)
{
    write_reg(0, 0x13);
    write_reg(1, 0x08);
    write_reg(1, 0x01);
    set_mask(0xff);

    this->cycle(2);
    this->dut.intr_in = (1 << 3);
    this->cycle(1);

    setup_watch_and_ack();
    while (!irqs_taken.size())
        cycle();

    write_reg(0, 0x20); // Non-specific EOI
    cycle(2);

    this->dut.intr_in |= (1 << 3);
    cycle(32);

    ASSERT_FALSE(this->dut.intr);
    ASSERT_EQ(irqs_taken.size(), 2LU);
    ASSERT_EQ(irqs_taken[0], 8 + 3);
    ASSERT_EQ(irqs_taken[1], 8 + 3);
}
