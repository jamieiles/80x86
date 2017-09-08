#include <gtest/gtest.h>
#include <VLoadStore.h>
#include <map>
#include <memory>

#include "VerilogTestbench.h"

using physaddr = uint32_t;

class LoadStoreTestbench : public VerilogTestbench<VLoadStore>,
                           public ::testing::Test
{
public:
    enum MemWidth { WIDTH_8, WIDTH_16 };

    LoadStoreTestbench();
    void add_memory(physaddr start, const std::vector<uint8_t> &bytes);
    std::vector<uint16_t> get_read_values() const
    {
        return read_values;
    }
    void write_mar(physaddr address)
    {
        after_n_cycles(0, [&] {
            this->dut.mar_in = address;
            this->dut.write_mar = 1;
            after_n_cycles(0, [&] {
                this->dut.write_mar = 0;
                this->dut.mar_in = 0;
            });
        });
        cycle();
    }
    void write_mdr(physaddr address)
    {
        after_n_cycles(0, [&] {
            this->dut.mdr_in = address;
            this->dut.write_mdr = 1;
            after_n_cycles(1, [&] {
                this->dut.write_mdr = 0;
                this->dut.mdr_in = 0;
            });
        });
        cycle();
    }
    void read(MemWidth width)
    {
        access(width, false);
    }
    void write(MemWidth width)
    {
        access(width, true);
    }
    std::map<physaddr, uint8_t> mem_bytes;

private:
    void access(MemWidth width, bool write)
    {
        after_n_cycles(0, [&] {
            this->dut.is_8bit = width == WIDTH_8;
            this->dut.start = 1;
            this->dut.wr_en = write;
            after_n_cycles(1, [&] { this->dut.start = 0; });
        });
        for (auto i = 0; i < 100; ++i) {
            cycle();
            if (dut.complete)
                return;
        }

        FAIL() << "failed to complete memory operation" << std::endl;
    }
    std::vector<uint16_t> read_values;
    bool reading;
    bool writing;
};

LoadStoreTestbench::LoadStoreTestbench() : reading(false), writing(false)
{
    reset();

    periodic(ClockCapture, [&] {
        if (!this->dut.reset && this->dut.complete && !this->dut.wr_en)
            read_values.push_back(this->dut.mdr_out);
    });
    periodic(ClockSetup, [&] {
        if (!this->dut.reset && this->dut.m_access && !reading &&
            !this->dut.wr_en) {
            reading = true;
            if (mem_bytes.find(this->dut.m_addr << 1) == mem_bytes.end())
                FAIL() << "no memory at 0x" << std::hex << this->dut.m_addr;
            after_n_cycles(4, [&] {
                this->dut.m_data_in =
                    mem_bytes[this->dut.m_addr << 1] |
                    (mem_bytes[(this->dut.m_addr << 1) + 1] << 8);
                this->dut.m_ack = 1;
                after_n_cycles(1, [&] {
                    this->dut.m_ack = 0;
                    reading = false;
                });
            });
        }
    });
    periodic(ClockSetup, [&] {
        if (!this->dut.reset && this->dut.m_access && !writing &&
            this->dut.wr_en) {
            writing = true;
            after_n_cycles(4, [&] {
                if (this->dut.m_bytesel & 0x1)
                    this->mem_bytes[this->dut.m_addr << 1] =
                        this->dut.m_data_out & 0xff;
                if (this->dut.m_bytesel & 0x2)
                    this->mem_bytes[(this->dut.m_addr << 1) + 1] =
                        (this->dut.m_data_out >> 8) & 0xff;
                this->dut.m_ack = 1;
                after_n_cycles(1, [&] {
                    this->dut.m_ack = 0;
                    writing = false;
                });
            });
        }
    });
}

void LoadStoreTestbench::add_memory(physaddr start,
                                    const std::vector<uint8_t> &bytes)
{
    auto addr = start;

    for (auto b : bytes)
        mem_bytes[addr++] = b;
}

TEST_F(LoadStoreTestbench, SegmentCalculation)
{
    this->dut.segment = 0x800;
    write_mar(0x100);

    cycle();

    ASSERT_EQ(this->dut.m_addr, ((0x800 << 4) + 0x100LU) >> 1);
}

TEST_F(LoadStoreTestbench, SegmentNoWrap)
{
    this->dut.segment = 0;
    add_memory(0xfffe, {0xee, 0x55});
    add_memory(0x10000, {0xaa, 0xff});

    write_mar(0xffff);
    read(WIDTH_16);

    ASSERT_EQ(get_read_values(), std::vector<uint16_t>{0xaa55});
}

struct LoadParams {
    physaddr mar_value;
    LoadStoreTestbench::MemWidth width;
    std::vector<uint16_t> values;
};

class LoadTest : public LoadStoreTestbench,
                 public ::testing::WithParamInterface<LoadParams>
{
};

TEST_P(LoadTest, LoadMemory)
{
    auto params = GetParam();

    add_memory(0xff, {0xff, 0x55, 0xaa, 0xff});

    write_mar(params.mar_value);
    read(params.width);

    ASSERT_EQ(get_read_values(), params.values);
}
INSTANTIATE_TEST_CASE_P(
    Load,
    LoadTest,
    ::testing::Values(
        LoadParams{0x100U, LoadStoreTestbench::MemWidth::WIDTH_16, {0xaa55}},
        LoadParams{0x101U, LoadStoreTestbench::MemWidth::WIDTH_16, {0xffaa}},
        LoadParams{0x100U, LoadStoreTestbench::MemWidth::WIDTH_8, {0x55}},
        LoadParams{0x101U, LoadStoreTestbench::MemWidth::WIDTH_8, {0xaa}}));

struct StoreParams {
    physaddr mar_value;
    uint16_t mdr_value;
    LoadStoreTestbench::MemWidth width;
    std::map<physaddr, uint8_t> expected_mem;
};

class StoreTest : public LoadStoreTestbench,
                  public ::testing::WithParamInterface<StoreParams>
{
};

TEST_P(StoreTest, StoreMemory)
{
    auto params = GetParam();

    write_mar(params.mar_value);
    write_mdr(params.mdr_value);

    write(params.width);

    ASSERT_TRUE(params.expected_mem.size() == mem_bytes.size() &&
                std::equal(params.expected_mem.begin(),
                           params.expected_mem.end(), mem_bytes.begin()));
}
INSTANTIATE_TEST_CASE_P(
    Store,
    StoreTest,
    ::testing::Values(StoreParams{0x100U,
                                  0xaa55,
                                  LoadStoreTestbench::MemWidth::WIDTH_16,
                                  {{0x100, 0x55}, {0x101, 0xaa}}},
                      StoreParams{0x101U,
                                  0xaa55,
                                  LoadStoreTestbench::MemWidth::WIDTH_16,
                                  {{0x101, 0x55}, {0x102, 0xaa}}},
                      StoreParams{0x100U,
                                  0xff55,
                                  LoadStoreTestbench::MemWidth::WIDTH_8,
                                  {{0x100, 0x55}}},
                      StoreParams{0x101U,
                                  0xff55,
                                  LoadStoreTestbench::MemWidth::WIDTH_8,
                                  {{0x101, 0x55}}}));
