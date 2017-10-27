// Copyright Jamie Iles, 2017
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
#include <VImmediateReader.h>
#include <deque>
#include <memory>

#include "VerilogTestbench.h"

class ImmediateReaderTestbench : public VerilogTestbench<VImmediateReader>,
                                 public ::testing::Test
{
public:
    ImmediateReaderTestbench();

    void add_bytes(const std::vector<uint8_t> bytes)
    {
        stream.insert(stream.end(), bytes.begin(), bytes.end());
        this->cycle();
    }

    void fetch()
    {
        after_n_cycles(0, [&] {
            this->dut.start = 1;
            after_n_cycles(1, [&] { this->dut.start = 0; });
        });

        for (auto i = 0; i < 1000; ++i) {
            cycle();
            if (complete)
                return;
        }

        FAIL() << "failed to complete immediate fetch" << std::endl;
    }

    bool is_complete() const
    {
        return complete;
    }

private:
    std::deque<uint8_t> stream;
    bool complete;
};

ImmediateReaderTestbench::ImmediateReaderTestbench() : complete(false)
{
    dut.reset = 0;
    reset();

    periodic(ClockSetup, [&] {
        this->dut.fifo_empty = this->stream.size() == 0;

        if (!this->dut.reset && this->dut.fifo_rd_en &&
            this->stream.size() == 0)
            FAIL() << "fifo underflow" << std::endl;

    });

    periodic(ClockCapture, [&] {
        this->complete = this->dut.complete;
        if (!this->dut.reset && this->dut.fifo_rd_en &&
            this->stream.size() > 0) {
            this->stream.pop_front();
        }
        after_n_cycles(1, [&] { this->dut.fifo_rd_data = this->stream[0]; });
    });
}

TEST_F(ImmediateReaderTestbench, Immed8PopsOne)
{
    add_bytes({0x80});
    dut.is_8bit = 1;

    fetch();

    EXPECT_EQ(dut.immediate, 0xff80LU);
}

TEST_F(ImmediateReaderTestbench, Immed8SignExtendPositive)
{
    add_bytes({0x7f});
    dut.is_8bit = 1;

    fetch();

    EXPECT_EQ(dut.immediate, 0x007f);
}

TEST_F(ImmediateReaderTestbench, Immed16PopsTwo)
{
    add_bytes({0xaa, 0x55});
    dut.is_8bit = 0;

    fetch();

    EXPECT_EQ(dut.immediate, 0x55aaLU);
}

TEST_F(ImmediateReaderTestbench, ImmedPersistsAfterCompletion)
{
    add_bytes({0xaa, 0x55});
    dut.is_8bit = 0;

    fetch();
    cycle(2);
    dut.fifo_rd_data = 0;
    cycle(2);

    EXPECT_EQ(dut.immediate, 0x55aaLU);
}
