#include <gtest/gtest.h>
#include <VPrefetch.h>
#include <vector>
#include <map>

#include "VerilogTestbench.h"

class PrefetchTestbench : public VerilogTestbench<VPrefetch> {
public:
    PrefetchTestbench(VPrefetch *dut);
    virtual void cycle(int count);
    std::vector<uint8_t> fifo_bytes;
    std::map<uint32_t, uint16_t> memory;
    int mem_latency;
    int mem_cycles_remaining;
};

PrefetchTestbench::PrefetchTestbench(VPrefetch *dut)
    : VerilogTestbench<VPrefetch>(dut)
{
    dut->reset = 0;
    dut->cs = 0;
    dut->new_ip = 0;
    dut->load_new_ip = 0;
    dut->fifo_full = 0;
    dut->mem_access = 0;
    dut->mem_ack = 0;
    dut->mem_data = 0;
    mem_latency = 1;
    mem_cycles_remaining = 0;
    reset();
}

void PrefetchTestbench::cycle(int count=1)
{
    for (auto i = 0; i < count; ++i) {
        if (!dut->reset && dut->fifo_wr_en)
            fifo_bytes.push_back(dut->fifo_wr_data & 0xff);

        // High edge
        VerilogTestbench<VPrefetch>::step();

        dut->eval();
        dut->mem_ack = 0;

        if (dut->mem_access && mem_cycles_remaining <= 0)
            mem_cycles_remaining = mem_latency;
        if (!dut->reset && dut->mem_access && --mem_cycles_remaining == 0) {
            if (memory.find(dut->mem_address) == memory.end())
                FAIL() << "no memory at 0x" << std::hex << dut->mem_address;
            dut->mem_data = memory[dut->mem_address];
            dut->mem_ack = 1;
        }

        // Low edge
        VerilogTestbench<VPrefetch>::step();

        dut->eval();
    }
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
    tb.memory[0x100] = 0x1234;
    tb.dut->new_ip = 0x101;
    tb.dut->load_new_ip = 1;
    tb.cycle();
    tb.dut->load_new_ip = 0;
    ASSERT_TRUE(tb.dut->fifo_reset);
    ASSERT_EQ(tb.dut->mem_address, 0x100LU);
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
    tb.memory[0x0000] = 0xffff;
    tb.memory[0xdead8] = 0xf00d;
    tb.dut->cs = 0xdead;
    tb.dut->new_ip = 0x8;
    tb.cycle(2);
    tb.dut->load_new_ip = 1;
    tb.cycle();
    tb.dut->load_new_ip = 0;
    tb.cycle(10);
    ASSERT_EQ(tb.fifo_bytes, (std::vector<uint8_t>{0x0d, 0xf0}));
}

TEST_F(PrefetchTestFixture, filled_fifo_doesnt_skip)
{
    for (uint32_t m = 0; m < 32; m += 2)
        tb.memory[m] = m;

    int pushed = 0;
    while (pushed != 6) {
        tb.cycle();
        if (tb.dut->fifo_wr_en)
            ++pushed;
    }
    tb.dut->fifo_full = 1;
    tb.cycle(128);
    tb.dut->fifo_full = 0;
    tb.cycle();

    pushed = 0;
    while (pushed != 2) {
        tb.cycle();
        if (tb.dut->fifo_wr_en)
            ++pushed;
    }

    ASSERT_EQ(tb.fifo_bytes, (std::vector<uint8_t>{0, 0, 2, 0, 4, 0, 6, 0}));
}
