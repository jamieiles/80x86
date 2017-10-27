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
#include <VSegmentRegisterFile.h>

#include "VerilogTestbench.h"
#include "RTLCPU.h"

class SegmentRegisterFileTestFixture
    : public VerilogTestbench<VSegmentRegisterFile>,
      public ::testing::Test
{
public:
    void write_sr(uint8_t sr, uint16_t v);
    void trigger_read_sr(uint8_t sr);
    uint16_t get_sr() const;
};

void SegmentRegisterFileTestFixture::write_sr(uint8_t sr, uint16_t v)
{
    dut.wr_sel = sr;
    dut.wr_val = v;
    dut.wr_en = 1;
    cycle();
    dut.wr_en = 0;
}

void SegmentRegisterFileTestFixture::trigger_read_sr(uint8_t sr)
{
    dut.rd_sel = sr;
    cycle();
}

uint16_t SegmentRegisterFileTestFixture::get_sr() const
{
    return dut.rd_val;
}

TEST_F(SegmentRegisterFileTestFixture, segment_read_write)
{
    for (auto r = 0; r < 4; ++r)
        write_sr(r, ~r);

    for (auto r = 0; r < 4; ++r) {
        trigger_read_sr(r);
        EXPECT_EQ(~r & 0xffff, get_sr());
    }
}

TEST_F(SegmentRegisterFileTestFixture, sr_read_during_write)
{
    write_sr(0, 0x1234);
    ASSERT_EQ(0x1234, get_sr());
}

TEST_F(SegmentRegisterFileTestFixture, cs_output_during_write)
{
    write_sr(1, 0xf00f);
    ASSERT_EQ(0xf00f, dut.cs);
}

class CoreFixture : public RTLCPU<verilator_debug_enabled>,
                    public ::testing::Test
{
public:
    CoreFixture() : RTLCPU(current_test_name())
    {
    }
};
TEST_F(CoreFixture, SegmentRegisterFileReset)
{
    for (auto r = static_cast<int>(ES); r <= static_cast<int>(DS); ++r) {
        write_reg(static_cast<GPR>(r), ~r);
        EXPECT_EQ(read_reg(static_cast<GPR>(r)), static_cast<uint16_t>(~r));
    }

    reset();

    for (auto r = static_cast<int>(ES); r <= static_cast<int>(DS); ++r)
        EXPECT_EQ(read_reg(static_cast<GPR>(r)),
                  static_cast<GPR>(r) == CS ? 0xffff : 0);
}
