#include <gtest/gtest.h>
#include <VLoadStore.h>
#include <map>
#include <memory>

#include "VerilogTestbench.h"

using physaddr = uint32_t;

class LoadStoreTestbench : public VerilogTestbench<VLoadStore>,
    public ::testing::Test {
public:
    LoadStoreTestbench();
    void add_memory(physaddr start, const std::vector<uint8_t> &bytes);
    std::vector<uint16_t> get_read_values() const
    {
        return read_values;
    }
    void write_mar(physaddr address)
    {
        after_n_cycles(0, [&]{
            this->dut.mar_in = address;
            this->dut.write_mar = 1;
            after_n_cycles(1, [&]{
                this->dut.write_mar = 0;
                this->dut.mar_in = 0;
            });
        });
        cycle();
    }
    void access()
    {
        after_n_cycles(0, [&]{
            this->dut.start = 1;
            after_n_cycles(1, [&]{
                this->dut.start = 0;
            });
        });
        for (auto i = 0; i < 100; ++i) {
            cycle();
            if (dut.complete)
                return;
        }

        FAIL() << "failed to complete memory operation" << std::endl;
    }
private:
    std::map<physaddr, uint8_t> mem_bytes;
    std::vector<uint16_t> read_values;
    bool reading;
};

LoadStoreTestbench::LoadStoreTestbench()
    : reading(false)
{
    reset();

    periodic(ClockCapture, [&]{
        if (!this->dut.reset && this->dut.complete)
            read_values.push_back(this->dut.mdr_out);
    });
    periodic(ClockSetup, [&]{
        if (!this->dut.reset && this->dut.m_access && !reading) {
            reading = true;
            if (mem_bytes.find(this->dut.m_addr << 1) == mem_bytes.end())
                FAIL() << "no memory at 0x" << std::hex << this->dut.m_addr;
            after_n_cycles(4, [&]{
                this->dut.m_data_in =
                    mem_bytes[this->dut.m_addr << 1] |
                    (mem_bytes[(this->dut.m_addr << 1) + 1] << 8);
                this->dut.m_ack = 1;
                after_n_cycles(1, [&]{
                    this->dut.m_ack = 0;
                    reading = false;
                });
            });
        }
    });
}

void LoadStoreTestbench::add_memory(physaddr start,
                                    const std::vector<uint8_t> &bytes)
{
    auto addr = start;

    for (auto b: bytes)
        mem_bytes[addr++] = b;
}

TEST_F(LoadStoreTestbench, SegmentCalculation)
{
    this->dut.segment = 0x800;
    write_mar(0x100);

    cycle();

    ASSERT_EQ(this->dut.m_addr, ((0x800 << 4) + 0x100LU) >> 1);
}

TEST_F(LoadStoreTestbench, Load16)
{
    add_memory(0x100, {0x55, 0xaa});

    write_mar(0x100);
    access();

    ASSERT_EQ(get_read_values(), std::vector<uint16_t>{0xaa55});
}

TEST_F(LoadStoreTestbench, Unaligned16Bit)
{
    add_memory(0x100, {0x00, 0x55, 0xaa});

    write_mar(0x101);
    access();

    ASSERT_EQ(get_read_values(), std::vector<uint16_t>{0xaa55});
}
