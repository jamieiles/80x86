#include <gtest/gtest.h>
#include <VPrefetch.h>
#include <vector>
#include <map>

#include "VerilogTestbench.h"

class PrefetchTestbench : public VerilogTestbench<VPrefetch> {
public:
    PrefetchTestbench(VPrefetch *dut);
    std::vector<uint8_t> fifo_bytes;
    std::map<uint32_t, uint16_t> memory;
    int mem_latency;
private:
    bool reading;
};

PrefetchTestbench::PrefetchTestbench(VPrefetch *dut)
    : VerilogTestbench<VPrefetch>(dut), mem_latency(0), reading(false)
{
    dut->reset = 0;
    dut->cs = 0;
    dut->new_ip = 0;
    dut->load_new_ip = 0;
    dut->fifo_full = 0;
    dut->mem_access = 0;
    dut->mem_ack = 0;
    dut->mem_data = 0;

    at_edge(PosEdge, [&]{
        if (!this->dut->reset && this->dut->fifo_wr_en)
            fifo_bytes.push_back(this->dut->fifo_wr_data & 0xff);
    });
    at_edge(NegEdge, [&]{
        this->dut->mem_ack = 0;
        this->dut->mem_data = 0;
        if (!this->dut->reset && this->dut->mem_access && !reading) {
            reading = true;
            if (memory.find(this->dut->mem_address) == memory.end())
                FAIL() << "no memory at 0x" << std::hex << this->dut->mem_address;
            after_n_cycles(mem_latency, [&]{
                this->dut->mem_data = memory[this->dut->mem_address];
                this->dut->mem_ack = 1;
                after_n_cycles(1, [&]{ this->dut->mem_ack = 0; reading = false; });
            });
        }
    });
    reset();
}

class PrefetchTestFixture : public ::testing::Test {
public:
    PrefetchTestFixture()
        : tb(&prefetch) {}
protected:
    VPrefetch prefetch;
    PrefetchTestbench tb;
};

TEST_F(PrefetchTestFixture, full_fifo_no_fetch)
{
    tb.dut->fifo_full = 1;
    tb.cycle();
    ASSERT_FALSE(tb.dut->mem_access);
}

TEST_F(PrefetchTestFixture, empty_fifo_triggers_fetch)
{
    tb.memory[0] = 0;
    tb.dut->fifo_full = 0;
    tb.cycle();
}

TEST_F(PrefetchTestFixture, fetch_writes_to_fifo)
{
    tb.memory[0] = 0xaa55;
    tb.memory[2] = 0xdead;
    tb.dut->fifo_full = 0;

    tb.cycle(3);

    auto expected = std::vector<uint8_t>{0x55, 0xaa};
    ASSERT_EQ(tb.fifo_bytes, expected);
}

TEST_F(PrefetchTestFixture, update_ip_resets_fifo)
{
    tb.memory[0x000] = 0xf00f;
    tb.memory[0x100] = 0x1234;
    tb.memory[0x102] = 0x0ff0;
    tb.dut->new_ip = 0x101;
    tb.dut->load_new_ip = 1;
    tb.cycle();
    tb.dut->load_new_ip = 0;
    ASSERT_TRUE(tb.dut->fifo_reset);
    ASSERT_EQ(tb.dut->mem_address, 0x100LU);
    tb.cycle(4);
    ASSERT_EQ(tb.fifo_bytes, std::vector<uint8_t>{0x12});
}

TEST_F(PrefetchTestFixture, odd_fetch_address_writes_one)
{
    tb.memory[0x000] = 0xbeef;
    tb.memory[0x100] = 0x1234;
    tb.memory[0x102] = 0xdead;
    tb.dut->new_ip = 0x101;
    tb.dut->load_new_ip = 1;
    tb.cycle();
    tb.dut->load_new_ip = 0;
    tb.cycle(4);

    ASSERT_EQ(tb.fifo_bytes, std::vector<uint8_t>{0x12});
}

TEST_F(PrefetchTestFixture, back_to_back_reads)
{
    tb.memory[0x000] = 0xbeef;
    tb.memory[0x100] = 0x12aa;
    tb.memory[0x102] = 0x5634;
    tb.memory[0x104] = 0xffff;
    tb.dut->new_ip = 0x101;
    tb.dut->load_new_ip = 1;
    tb.cycle();
    tb.dut->load_new_ip = 0;
    // Start fetching from the new address, this should do back to back reads,
    // so 6 cycles to write the 3 bytes.
    tb.cycle(6);

    ASSERT_EQ(tb.fifo_bytes, (std::vector<uint8_t>{0x12, 0x34, 0x56}));
}

TEST_F(PrefetchTestFixture, address_generation)
{
    tb.memory[0xdead8] = 0xf00d;
    tb.dut->cs = 0xdead;
    tb.dut->new_ip = 0x8;
    tb.dut->load_new_ip = 1;
    tb.cycle();
    ASSERT_EQ(tb.dut->mem_address, 0xdead8LU);
}

TEST_F(PrefetchTestFixture, new_ip_discards_current_fetch)
{
    tb.mem_latency = 5;
    tb.memory[0xdead0] = 0xffff;
    tb.memory[0xdead8] = 0xf00d;
    tb.memory[0xdeada] = 0xface;
    tb.memory[0xdeadc] = 0x5555;
    tb.dut->cs = 0xdead;
    tb.dut->new_ip = 0x8;
    tb.cycle(2);
    tb.dut->load_new_ip = 1;
    tb.cycle();
    tb.dut->load_new_ip = 0;
    tb.cycle(20);
    ASSERT_EQ(tb.fifo_bytes, (std::vector<uint8_t>{0x0d, 0xf0, 0xce, 0xfa}));
}

TEST_F(PrefetchTestFixture, filled_fifo_doesnt_skip)
{
    for (uint32_t m = 0; m < 32; m += 2)
        tb.memory[m] = m;

    while (tb.fifo_bytes.size() != 6)
        tb.cycle();
    tb.dut->fifo_full = 1;
    tb.cycle(128);
    tb.dut->fifo_full = 0;
    tb.cycle();

    while (tb.fifo_bytes.size() != 8)
        tb.cycle();

    ASSERT_EQ(tb.fifo_bytes, (std::vector<uint8_t>{0, 0, 2, 0, 4, 0, 6, 0}));
}
