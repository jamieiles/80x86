// Copyright Jamie Iles, 2018
//
// This file is part of s80x86.
//
// s80x86 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// s80x86 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with s80x86.  If not, see <http://www.gnu.org/licenses/>.

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <VCache.h>
#include <map>
#include <memory>

#include "VerilogTestbench.h"

using physaddr = uint32_t;

struct Write {
public:
    uint32_t address;
    uint16_t val;

    bool operator==(const Write &rhs) const
    {
        return address == rhs.address && val == rhs.val;
    }
};

class CacheTestbench : public VerilogTestbench<VCache>, public ::testing::Test
{
public:
    enum MemWidth { WIDTH_8, WIDTH_16 };

    CacheTestbench();
    void add_memory(physaddr start, const std::vector<uint8_t> &bytes);
    std::vector<uint16_t> get_read_values() const
    {
        return read_values;
    }

    std::vector<Write> get_writes() const
    {
        return writes;
    }

    std::vector<uint32_t> get_fetched_addresses() const
    {
        return fetched_addresses;
    }

    void read(uint32_t addr)
    {
        access(addr, false);
    }
    void write(uint32_t addr, uint16_t v, uint8_t bytesel = 3)
    {
        access(addr, true, v, bytesel);
    }
    std::map<physaddr, uint8_t> mem_bytes;

private:
    void access(uint32_t addr,
                bool write,
                uint16_t data = 0,
                uint8_t bytesel = 3)
    {
        after_n_cycles(0, [&] {
            this->dut.c_addr = addr >> 1;
            this->dut.c_data_out = data;
            this->dut.c_access = 1;
            this->dut.c_wr_en = write;
            this->dut.c_bytesel = bytesel & 0x3;
        });
        for (auto i = 0; i < 100; ++i) {
            cycle();
            if (dut.c_ack)
                return;
        }

        FAIL() << "failed to complete memory operation" << std::endl;
    }
    std::vector<uint16_t> read_values;
    std::vector<uint32_t> fetched_addresses;
    std::vector<Write> writes;
    bool reading;
    bool writing;
};

CacheTestbench::CacheTestbench() : reading(false), writing(false)
{
    this->dut.enabled = 1;

    reset();

    periodic(ClockCapture, [&] {
        if (!this->dut.reset && this->dut.c_ack && !this->dut.c_wr_en)
            read_values.push_back(this->dut.c_data_in);
        if (this->dut.c_ack)
            this->dut.c_access = 0;
    });
    periodic(ClockSetup, [&] {
        if (!this->dut.reset && this->dut.m_access && !reading &&
            !this->dut.m_wr_en) {
            reading = true;
            if (mem_bytes.find(this->dut.m_addr << 1) == mem_bytes.end())
                FAIL() << "no memory at 0x" << std::hex << this->dut.m_addr;
            this->fetched_addresses.push_back(this->dut.m_addr << 1);
            after_n_cycles(2, [&] {
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
            this->dut.m_wr_en) {
            writing = true;
            this->writes.push_back(
                Write{this->dut.m_addr << 1, this->dut.m_data_out});
            after_n_cycles(2, [&] {
                this->mem_bytes[this->dut.m_addr << 1] =
                    this->dut.m_data_out & 0xff;
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

void CacheTestbench::add_memory(physaddr start,
                                const std::vector<uint8_t> &bytes)
{
    auto addr = start;

    for (auto b : bytes)
        mem_bytes[addr++] = b;
}

TEST_F(CacheTestbench, ReadFetchesLine)
{
    add_memory(0x100, {0xa5, 0xa5});
    for (int i = 0; i < 7; ++i)
        add_memory(0x102 + i * 2, {0x00, 0x00});

    read(0x100);
    cycle(256);

    EXPECT_EQ(get_read_values(), std::vector<uint16_t>{0xa5a5});

    std::vector<uint32_t> addresses;
    for (int i = 0; i < 8; ++i)
        addresses.push_back(0x0100 + i * 2);
    EXPECT_EQ(get_fetched_addresses(), addresses);
}

TEST_F(CacheTestbench, TwoLineFetches)
{
    for (uint8_t i = 0; i < 16; ++i)
        add_memory(0x100 + i * 2, {i, i});

    read(0x100);
    read(0x110);
    cycle(256);

    EXPECT_EQ(get_read_values(), std::vector<uint16_t>({0x0000, 0x0808}));

    std::vector<uint32_t> addresses;
    for (int i = 0; i < 16; ++i)
        addresses.push_back(0x0100 + i * 2);
    EXPECT_EQ(get_fetched_addresses(), addresses);
}

TEST_F(CacheTestbench, WriteFetchesLine)
{
    for (int i = 0; i < 8; ++i)
        add_memory(0x100 + i * 2, {0x00, 0x00});

    write(0x100, 0xa5a5);
    cycle(256);

    EXPECT_EQ(get_read_values(), std::vector<uint16_t>{});

    std::vector<uint32_t> addresses;
    for (int i = 0; i < 8; ++i)
        addresses.push_back(0x0100 + i * 2);
    EXPECT_EQ(get_fetched_addresses(), addresses);
}

TEST_F(CacheTestbench, WriteModifies)
{
    for (int i = 0; i < 8; ++i)
        add_memory(0x100 + i * 2, {0x00, 0x00});

    write(0x102, 0xaa55);
    read(0x102);

    EXPECT_EQ(get_read_values(), std::vector<uint16_t>{0xaa55});
}

TEST_F(CacheTestbench, WriteBytes)
{
    for (int i = 0; i < 8; ++i)
        add_memory(0x100 + i * 2, {0x00, 0x00});

    write(0x102, 0x0055, 1);
    write(0x102, 0xaa00, 2);
    read(0x102);
    cycle(128);

    EXPECT_EQ(get_read_values(), std::vector<uint16_t>{0xaa55});
}

TEST_F(CacheTestbench, NonCacheAlignedRead)
{
    for (int i = 0; i < 8; ++i)
        add_memory(0x100 + i * 2, {0x00, 0x00});
    add_memory(0x104, {0xa5, 0xa5});

    read(0x104);
    cycle(256);

    EXPECT_EQ(get_read_values(), std::vector<uint16_t>{0xa5a5});
}

TEST_F(CacheTestbench, ReadTwiceSameLine)
{
    for (int i = 0; i < 8; ++i)
        add_memory(0x100 + i * 2, {static_cast<uint8_t>(i * 2), 0x00});

    read(0x106);
    read(0x104);
    cycle(256);

    EXPECT_EQ(get_read_values(), std::vector<uint16_t>({0x0006, 0x0004}));
}

TEST_F(CacheTestbench, RepeatedReadNoRefetch)
{
    for (int i = 0; i < 8; ++i)
        add_memory(0x100 + i * 2, {static_cast<uint8_t>(i * 2), 0x00});

    for (int i = 0; i < 3; ++i) {
        read(0x100);
        cycle(256);
    }
    cycle(1024);

    EXPECT_EQ(get_read_values(), std::vector<uint16_t>({0, 0, 0}));

    std::vector<uint32_t> addresses;
    for (int i = 0; i < 8; ++i)
        addresses.push_back(0x0100 + i * 2);
    EXPECT_EQ(get_fetched_addresses(), addresses);
}

TEST_F(CacheTestbench, ReadConflictFetches)
{
    for (int i = 0; i < 8; ++i) {
        add_memory(0x100 + i * 2, {static_cast<uint8_t>(i * 2), 0x00});
        add_memory(0x200 + i * 2, {static_cast<uint8_t>(i * 2), 0x02});
    }

    read(0x100);
    read(0x200);
    cycle(1024);

    EXPECT_EQ(get_read_values(), std::vector<uint16_t>({0x0000, 0x0200}));

    std::vector<uint32_t> addresses;
    for (int i = 0; i < 8; ++i)
        addresses.push_back(0x0100 + i * 2);
    for (int i = 0; i < 8; ++i)
        addresses.push_back(0x0200 + i * 2);

    EXPECT_EQ(get_fetched_addresses(), addresses);
}

TEST_F(CacheTestbench, ReadConflictFlushesDirty)
{
    for (int i = 0; i < 8; ++i) {
        add_memory(0x100 + i * 2, {0x00, 0x00});
        add_memory(0x10100 + i * 2, {0x00, 0x02});
    }

    write(0x100, 0xa5a5);
    cycle(256);
    read(0x10100);
    cycle(256);

    EXPECT_EQ(get_read_values(), std::vector<uint16_t>{0x0200});

    std::vector<Write> writes;
    writes.push_back({0x0100, 0xa5a5});
    for (uint32_t i = 1; i < 8; ++i)
        writes.push_back({0x0100 + i * 2, 0});
    EXPECT_THAT(writes, ::testing::ContainerEq(get_writes()));

    std::vector<uint32_t> addresses;
    for (int i = 0; i < 8; ++i)
        addresses.push_back(0x0100 + i * 2);
    for (int i = 0; i < 8; ++i)
        addresses.push_back(0x10100 + i * 2);
    EXPECT_EQ(get_fetched_addresses(), addresses);
}

TEST_F(CacheTestbench, FlushDirtyLine)
{
    for (int i = 0; i < 8; ++i) {
        add_memory(0x100 + i * 2, {0x00, 0x00});
        add_memory(0x10100 + i * 2, {0x00, 0x02});
    }

    for (int i = 0; i < 8; ++i)
        write(0x100 + i * 2, ~i);
    read(0x10100);
    cycle(256);

    EXPECT_EQ(get_read_values(), std::vector<uint16_t>{0x0200});

    std::vector<Write> writes;
    for (uint32_t i = 0; i < 8; ++i)
        writes.push_back({0x0100 + i * 2, static_cast<uint16_t>(~i)});
    EXPECT_THAT(writes, ::testing::ContainerEq(get_writes()));
}
