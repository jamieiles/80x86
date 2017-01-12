#include <gtest/gtest.h>
#include <VImmediateReader.h>
#include <deque>
#include <memory>

#include "VerilogTestbench.h"

class ImmediateReaderTestbench : public VerilogTestbench<VImmediateReader>,
    public ::testing::Test {
public:
    ImmediateReaderTestbench();

    void add_bytes(const std::vector<uint8_t> bytes)
    {
        stream.insert(stream.end(), bytes.begin(), bytes.end());
    }

    void fetch()
    {
        dut.start = 1;
        cycle();
        dut.start = 0;

        for (auto i = 0; i < 1000; ++i) {
            cycle();
            if (dut.complete)
                return;
        }

        FAIL() << "failed to complete immediate fetch" << std::endl;
    }

private:
    std::deque<uint8_t> stream;
};

ImmediateReaderTestbench::ImmediateReaderTestbench()
{
    dut.reset = 0;
    reset();

    periodic(ClockSetup, [&]{
        this->dut.fifo_empty = this->stream.size() == 0;
        if (!this->dut.reset && this->dut.fifo_rd_en &&
            this->stream.size() == 0)
            FAIL() << "fifo underflow" << std::endl;

        if (!this->dut.reset && this->dut.fifo_rd_en &&
            this->stream.size() > 0) {
            after_n_cycles(0, [&]{
                this->dut.fifo_empty = this->stream.size() == 0;
                this->dut.fifo_rd_data = this->stream.size() > 0 ?
                    this->stream[0] : 0;
                this->stream.pop_front();
            });
        }
    });
}

TEST_F(ImmediateReaderTestbench, Immed8PopsOne)
{
    add_bytes({ 0x80 });
    dut.is_8bit = 1;

    fetch();

    ASSERT_EQ(dut.immediate, 0xff80LU);
}

TEST_F(ImmediateReaderTestbench, Immed8SignExtendPositive)
{
    add_bytes({ 0x7f });
    dut.is_8bit = 1;

    fetch();

    ASSERT_EQ(dut.immediate, 0x007f);
}

TEST_F(ImmediateReaderTestbench, Immed16PopsTwo)
{
    add_bytes({ 0xaa, 0x55 });
    dut.is_8bit = 0;

    fetch();

    ASSERT_EQ(dut.immediate, 0x55aaLU);
}

TEST_F(ImmediateReaderTestbench, ImmedPersistsAfterCompletion)
{
    add_bytes({ 0xaa, 0x55 });
    dut.is_8bit = 0;

    fetch();
    cycle(2);
    dut.fifo_rd_data = 0;
    cycle(2);

    ASSERT_EQ(dut.immediate, 0x55aaLU);
}
